#include "LinuxResProvider.hpp"

/**
 *	@brief	Default constructor for LinuxResProvider.
 *	Initializes first CPU usage measurements.
 */
LinuxResProvider::LinuxResProvider()
{
	initSystemCpuUsage();
	initSelfCpuUsage();
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
int LinuxResProvider::parseLine(char* line)
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
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}

/**
 *	@brief	Method that saves initial values of total cpu time from
 *	the system boot for all kind of processes.
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
        //Overflow detection. Just skip this value.
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
 */
double LinuxResProvider::getSelfCpuUsage(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
        timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
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
	return hddState;
}
