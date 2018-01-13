#include "WindowsResProvider.hpp"

#ifdef _WIN32

WindowsResProvider::WindowsResProvider()
{
	initSystemCpuUsage();
	initSelfCpuUsage();
	initHddUsage();
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
	return hddState;
}

std::pair<double, double> WindowsResProvider::getHddSelfUsage(void)
{
	return std::pair<double, double>(1.0, 2.0);
}

std::pair<double, double> WindowsResProvider::getHddSystemUsage(void)
{
	return std::pair<double, double>(1.0, 2.0);
}

void WindowsResProvider::initHddUsage(void)
{

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

	self = GetCurrentProcess();
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
