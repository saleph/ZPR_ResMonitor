/*
 * WindowsResProvider.cpp
 *
 *  Created on: 11 sty 2018
 *      Author: Piotr
 */

#include "WindowsResProvider.hpp"

WindowsResProvider::WindowsResProvider() {

}

CpuState getCpuState(void)
{
	CpuState cpuState;
	return cpuState;
}

/**
 * 	@brief	Method that provides RamState object in Windows system
 * 	according to current RAM memory state.
 *
 * 	@return	RamState object with current RAM memory usage state.
 */
RamState getRamState(void)
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
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	ramState.setMonitorMBUsed(physMemUsedByMe/1000000.0);

	return ramState;
}

HddState getHddState(void)
{
	HddState hddState;
	return hddState;
}

std::pair<double, double> getHddSelfUsage(void)
{
	return std::pair<double, double>(1.0, 2.0);
}

std::pair<double, double> getHddSystemUsage(void)
{
	return std::pair<double, double>(1.0, 2.0);
}

void initHddUsage(void)
{

}

int parseLineRam(char* line)
{

}

int getRamSelfUsage(void)	// Value in KB!
{

}

void initSystemCpuUsage(void)
{

}

double getSystemCpuUsage(void)
{

}

void initSelfCpuUsage(void)
{

}

double getSelfCpuUsage(void)
{

}
