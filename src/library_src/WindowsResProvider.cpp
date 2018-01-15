#include "WindowsResProvider.hpp"

#ifdef _WIN32

/**
 *	@brief	Default constructor for WindowsResProvider.
 *	Initializes first CPU and Hdd usage measurements.
 */
WindowsResProvider::WindowsResProvider()
{
	self = GetCurrentProcess();
	initSystemCpuUsage();
	initSelfCpuUsage();
	initHddUsage();
	getCpuState();
}

/**
 * 	@brief	Method that provides CpuState object in Windows system
 * 	according to current CPU state.
 *
 * 	@return	CpuState object with current CPU usage state.
 */
CpuState WindowsResProvider::getCpuState(void)
{
	CpuState cpuState;
	cpuState.setPercentageUsed(getSystemCpuUsage());
	cpuState.setMonitorPercentageUsed(getSelfCpuUsage());
	return cpuState;
}

/**
 * 	@brief	Method that provides RamState object in Windows system
 * 	according to current RAM memory state.
 *
 * 	@return	RamState object with current RAM memory usage state.
 */
RamState WindowsResProvider::getRamState(void)
{
	RamState ramState;

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	// Get total RAM memory
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	// Get RAM memory used
	DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	ramState.setTotalMB(totalPhysMem/1000000.0);
	ramState.setMBUsed(physMemUsed/1000000.0);

	// Get memory used by current process
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	ramState.setMonitorMBUsed(physMemUsedByMe/1000000.0);

	return ramState;
}

/**
 * 	@brief	Method that provides HddState object in Windows system
 * 	according to current hard drive bandwidth usage state.
 *
 * 	@return	HddState object with current hard drive bandwidth usage state.
 */
HddState WindowsResProvider::getHddState(void)
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

/**
 * 	@brief	Method which reads current disk bandwidth usage for
 * 	this process for read and write operations in KB/s.
 *
 * 	@return	std::pair<double, double> objects in which 1st value is
 * 	current disk read bandwidth usage [KB/s], 2nd is current disk write
 * 	bandwidth usage [KB/s]
 */
std::pair<double, double> WindowsResProvider::getHddSelfUsage(void)
{
	IO_COUNTERS ioCounters;
	long long readBytes, writeBytes;
	double readKBs, writeKBs;

	if(GetProcessIoCounters(self, &ioCounters) == 0)
	{
		std::cerr<<"Failed to get IoCounters for Windows self-process..."<<std::endl;
		return std::pair<double, double>(-1.0, -1.0);
	}
	readBytes = ioCounters.ReadTransferCount;
	writeBytes = ioCounters.WriteTransferCount;
	milliseconds currMs = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	readKBs = (readBytes - lastHddSelfRead)/
			(double)((currMs - hddSelfLastMeasureTime).count());
	writeKBs = (writeBytes - lastHddSelfWrite)/
			(double)((currMs - hddSelfLastMeasureTime).count());
	hddSelfLastMeasureTime = currMs;
	lastHddSelfRead = readBytes;
	lastHddSelfWrite = writeBytes;

	return std::pair<double, double>(readKBs, writeKBs);
}

/**
 * 	@brief	Method which reads current disk bandwidth usage in the
 * 	system for read and write operations in KB/s.
 *
 * 	@return	std::pair<double, double> objects in which 1st value is
 * 	current disk read bandwidth usage [KB/s], 2nd is current disk write
 * 	bandwidth usage [KB/s]
 */
std::pair<double, double> WindowsResProvider::getHddSystemUsage(void)
{
	unsigned long long writeBytes, readBytes;
	double readKBs, writeKBs;
	DISK_PERFORMANCE diskPerformance;

	LPCTSTR hddPath = "\\\\.\\PhysicalDrive0";
	if(GetDrivePerformance(hddPath, &diskPerformance) == 0)
	{
		std::cerr<<"Failed to get disk performance structure for Windows..."<<std::endl;
		return std::pair<double, double>(-1.0, -1.0);
	}
	readBytes = diskPerformance.BytesRead.QuadPart;
	writeBytes = diskPerformance.BytesWritten.QuadPart;

	milliseconds currMs = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	readKBs = (readBytes - lastHddSystemRead)/
			(double)((currMs - hddSystemLastMeasureTime).count());
	writeKBs = (writeBytes - lastHddSystemWrite)/
			(double)((currMs - hddSystemLastMeasureTime).count());

	hddSystemLastMeasureTime = currMs;
	lastHddSystemRead = readBytes;
	lastHddSystemWrite = writeBytes;

	return std::pair<double, double>(readKBs, writeKBs);
}

/**
 * 	@brief	This method initializes resource provider members used
 * 	to store hdd usage information between resource getter calls.
 */
void WindowsResProvider::initHddUsage(void)
{
	IO_COUNTERS ioCounters;
	DISK_PERFORMANCE diskPerformance;
	if(GetProcessIoCounters(self, &ioCounters) == 0)
	{
		std::cerr<<"Failed to get IoCounters for Windows self-process..."<<std::endl;
		return;
	}
	lastHddSelfRead = ioCounters.ReadTransferCount;
	lastHddSelfWrite = ioCounters.WriteTransferCount;

	LPCTSTR hddPath = "\\\\.\\PhysicalDrive0";
	if(GetDrivePerformance(hddPath, &diskPerformance) == 0)
	{
		std::cerr<<"Failed to get disk performance structure for Windows..."<<std::endl;
		return;
	}
	lastHddSystemRead = diskPerformance.BytesRead.QuadPart;
	lastHddSystemWrite = diskPerformance.BytesWritten.QuadPart;
	hddSystemLastMeasureTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	hddSelfLastMeasureTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
}

/**
 * 	@brief	Method that reads current hard drive performance.
 *
 * 	@param	devPath is name of the device which performance is to be captured
 * 	@param	pdp is pointer to DISK_PERFORMANCE structure
 *
 * 	@return	true if data succesfully read, false otherwise.
 */
BOOL WindowsResProvider::GetDrivePerformance(LPCTSTR devPath, DISK_PERFORMANCE *pdp)
{
  HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined
  BOOL bResult   = FALSE;                 // results flag
  DWORD junk     = 0;                     // discard results

  hDevice = CreateFile(devPath,          // drive to open
                        0,                // no access to the drive
                        FILE_SHARE_READ | // share mode
                        FILE_SHARE_WRITE,
                        NULL,             // default security attributes
                        OPEN_EXISTING,    // disposition
                        0,                // file attributes
                        NULL);            // do not copy file attributes

  if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
  {
    return (FALSE);
  }

  bResult = DeviceIoControl(hDevice,                       // device to be queried
		  	  	  	  	  	IOCTL_DISK_PERFORMANCE, // operation to perform
                            NULL, 0,                       // no input buffer
                            pdp, sizeof(*pdp),            // output buffer
                            &junk,                         // # bytes returned
                            (LPOVERLAPPED) NULL);          // synchronous I/O

  CloseHandle(hDevice);

  return (bResult);
}

/**
 *	@brief	Method that saves initial values of total cpu time from
 *	the system boot for all processes.
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
void WindowsResProvider::initSystemCpuUsage(void)
{
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}

/**
 *	@brief	Method that returns system cpu usage percentage
 *
 *	@return	percent is double, it is percent of the current
 *	cpu usage by the Windows system with all applications
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
double WindowsResProvider::getSystemCpuUsage(void)
{
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
}

/**
 *	@brief	Method that saves initial values of total cpu time for
 *	the current process.
 *
 *	Code based on
 * 	https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */
void WindowsResProvider::initSelfCpuUsage(void)
{
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
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
double WindowsResProvider::getSelfCpuUsage(void)
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100.0;
}
#endif
