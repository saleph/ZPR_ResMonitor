#include "LinuxResProvider.hpp"

#ifdef __linux__

/**
 *	@brief	Default constructor for LinuxResProvider.
 *	Initializes first CPU and Hdd usage measurements.
 */
LinuxResProvider::LinuxResProvider()
{
	initSystemCpuUsage();
	initSelfCpuUsage();
	initHddUsage();
	getCpuState();
}

/**
 * 	@brief	This method initializes resource provider members used
 * 	to store resource usage information between resource getter calls.
 */
void LinuxResProvider::initHddUsage(void)
{
	FILE * sectorSizeFile = fopen("/sys/block/sda/queue/hw_sector_size", "r");
	fscanf(sectorSizeFile, "%d", &hddSectorSize);
	fclose(sectorSizeFile);
	getHddSystemUsage();
	getHddSelfUsage();
}

/**
 * 	@brief	Method which reads current disk bandwidth usage in the
 * 	system for read and write operations in KB/s.
 *
 * 	@return	std::pair<double, double> objects in which 1st value is
 * 	current disk read bandwidth usage [KB/s], 2nd is current disk write
 * 	bandwidth usage [KB/s]
 */
std::pair<double, double> LinuxResProvider::getHddSystemUsage(void)
{
	FILE * ioStats = fopen("/proc/diskstats", "r");

	unsigned long long writeBytes, readBytes;
	double readKBs, writeKBs;
	char line[128];
	char * pch;

	while (fgets(line, 128, ioStats) != NULL){
		if ((pch = strstr(line, "sda ")) != NULL){
			pch = strtok(pch, " \t");
			int i = 0;
			do
			{
				if(i == 3)
				{
					readBytes = std::stoll(pch, nullptr, 10);
					readBytes *= hddSectorSize;
				}
				else if(i == 7)
				{
					writeBytes = std::stoll(pch, nullptr, 10);
					writeBytes *= hddSectorSize;
					break;
				}
				pch = strtok (NULL, " \t");
			}while(++i < 8);
			break;
		}
	}
	milliseconds currMs = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	readKBs = (readBytes - lastHddSystemRead)/
			(double)((currMs - hddSystemLastMeasureTime).count());
	writeKBs = (writeBytes - lastHddSystemWrite)/
			(double)((currMs - hddSystemLastMeasureTime).count());

	hddSystemLastMeasureTime = currMs;
	lastHddSystemRead = readBytes;
	lastHddSystemWrite = writeBytes;

	fclose(ioStats);
	return std::pair<double, double>(readKBs, writeKBs);
}

/**
 * 	@brief	Method which reads current disk bandwidth usage for
 * 	this process for read and write operations in KB/s.
 *
 * 	@return	std::pair<double, double> objects in which 1st value is
 * 	current disk read bandwidth usage [KB/s], 2nd is current disk write
 * 	bandwidth usage [KB/s]
 */
std::pair<double, double> LinuxResProvider::getHddSelfUsage(void)
{
	FILE * ioStats = fopen("/proc/self/io", "r");

	long long writeBytes, readBytes;
	double readKBs, writeKBs;
	char line[64];
	while (fgets(line, 64, ioStats) != NULL){
		if (strncmp(line, "rchar:", 6) == 0){
			readBytes = std::stoll(&line[6], nullptr, 10);
		}
		else if(strncmp(line, "wchar:", 6) == 0){
			writeBytes = std::stoll(&line[6], nullptr, 10);
			break;
		}
	}
	milliseconds currMs = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	readKBs = (readBytes - lastHddSelfRead)/
			(double)((currMs - hddSelfLastMeasureTime).count());
	writeKBs = (writeBytes - lastHddSelfWrite)/
			(double)((currMs - hddSelfLastMeasureTime).count());
	hddSelfLastMeasureTime = currMs;
	lastHddSelfRead = readBytes;
	lastHddSelfWrite = writeBytes;

	fclose(ioStats);
	return std::pair<double, double>(readKBs, writeKBs);
}


/**
 * 	@brief	Method that search for number value
 * 	in given line, skipping last 3 characters (unit mark + space).
 *
 * 	@param	line is a pointer to characters array (line to be parsed).
 *	@return	number read from given line.
 *
 * Code based on
 * https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
int LinuxResProvider::parseLineRam(char* line)
{
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

/**
 * 	@brief	Method that reads current RAM usage by this application.
 *
 * 	@return	RAM usage in KB.
 *
 * 	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
int LinuxResProvider::getRamSelfUsage(void)
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLineRam(line);
            break;
        }
    }
    fclose(file);
    return result;
}

/**
 *	@brief	Method that saves initial values of total cpu time from
 *	the system boot for all processes.
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
void LinuxResProvider::initSystemCpuUsage()
{
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalNiced,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

/**
 *	@brief	Method that returns system cpu usage percentage
 *
 *	@return	percent is double, it is percent of the current
 *	cpu usage by the Linux system with all applications
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
double LinuxResProvider::getSystemCpuUsage()
{
    double percent;
    FILE* file;
    unsigned long long totalUser, totalNiced, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalNiced,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalNiced < lastTotalNiced ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle)
    {
        //Overflow detection. Just skip this triggerValue.
        percent = -1.0;
    }
    else
    {
        total = (totalUser - lastTotalUser) + (totalNiced - lastTotalNiced) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalNiced = totalNiced;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

/**
 *	@brief	Method that saves initial values of total cpu time fromLinuxResProvider
 *	the current process.
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
void LinuxResProvider::initSelfCpuUsage(){
    FILE* file;
    struct tms timeSample;
    char line[128];

    lastCPU = times(&timeSample);	// times() gets process times
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file);
}

/**
 *	@brief	Method that returns this process cpu usage percentage
 *
 *	@return	percent is double, it is percent of the current
 *	cpu usage by this process.
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
double LinuxResProvider::getSelfCpuUsage(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
        timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this triggerValue.
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) +
            (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return percent;
}


/**
 * 	@brief	Method that provides CpuState object in Linux system
 * 	according to current CPU state.
 *
 * 	@return	CpuState object with current CPU usage state.
 */
CpuState LinuxResProvider::getCpuState(void)
{
	CpuState cpuState;
	cpuState.setPercentageUsed(getSystemCpuUsage());
	cpuState.setMonitorPercentageUsed(getSelfCpuUsage());
	return cpuState;
}

/**
 * 	@brief	Method that provides RamState object in Linux system
 * 	according to current RAM memory state.
 *
 * 	@return	RamState object with current RAM memory usage state.
 */
RamState LinuxResProvider::getRamState(void)
{
	RamState ramState;
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalPhysMem = memInfo.totalram;

	totalPhysMem *= memInfo.mem_unit;
	ramState.setTotalMB(totalPhysMem/(1024.0 * 1024.0));

	long long physMemUsed = memInfo.totalram - memInfo.freeram;
	//Multiply in next statement to avoid int overflow on right hand side...
	physMemUsed *= memInfo.mem_unit;
	ramState.setMBUsed(physMemUsed/(1024.0 * 1024.0));

	ramState.setMonitorMBUsed(getRamSelfUsage()/1024.0);
	return ramState;
}

/**
 * 	@brief	Method that provides HddState object in Linux system
 * 	according to current hard drive bandwidth usage state.
 *
 * 	@return	HddState object with current hard drive bandwidth usage state.
 */
HddState LinuxResProvider::getHddState(void)
{
	HddState hddState;
	std::pair<double, double> systemHddUsage = getHddSystemUsage();
	std::pair<double, double> selfHddUsage = getHddSelfUsage();
	hddState.setKBsUsedRead(systemHddUsage.first);
	hddState.setKBsUsedWrite(systemHddUsage.second);
	hddState.setMonitorKBsUsedRead(selfHddUsage.first);
	hddState.setMonitorKBsUsedWrite(selfHddUsage.second);

	return hddState;
}
#endif /* __linux__ */
