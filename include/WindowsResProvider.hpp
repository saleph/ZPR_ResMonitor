#ifndef INCLUDE_WINDOWSRESPROVIDER_HPP_
#define INCLUDE_WINDOWSRESPROVIDER_HPP_


#ifdef _WIN32

#include "ResUsageProvider.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>
#include <utility>
#include <iostream>
#include <chrono>
#include "windows.h"
#include <winioctl.h>
#include "psapi.h"
#include "TCHAR.h"
#include "pdh.h"

using namespace std::chrono;

/**
 * 	Class that provides states of resources in Windows system
 * 	(CPU, RAM and Hard Drive). Can be only used on Windows systems.
 * 	Corresponding resource provider class for Linux is LinuxResProvider.
 */
class WindowsResProvider : public ResUsageProvider
{
public:
	/**
	 *	@brief	Default constructor for WindowsResProvider.
	 *	Initializes first CPU and Hdd usage measurements.
	 */
	WindowsResProvider();

	/**
	 * 	@brief	Method that provides CpuState object in Windows system
	 * 	according to current CPU state.
	 *
	 * 	@return	CpuState object with current CPU usage state.
	 */
	CpuState getCpuState(void);

	/**
	 * 	@brief	Method that provides RamState object in Windows system
	 * 	according to current RAM memory state.
	 *
	 * 	@return	RamState object with current RAM memory usage state.
	 */
	RamState getRamState(void);

	/**
	 * 	@brief	Method that provides HddState object in Windows system
	 * 	according to current hard drive bandwidth usage state.
	 *
	 * 	@return	HddState object with current hard drive bandwidth usage state.
	 */
	HddState getHddState(void);

private:
	BOOL GetDrivePerformance(LPCTSTR wszPath, DISK_PERFORMANCE *pdp);

	void initSystemCpuUsage(void);
	double getSystemCpuUsage(void);
	void initSelfCpuUsage(void);
	double getSelfCpuUsage(void);
	void initHddUsage(void);
	std::pair<double, double> getHddSelfUsage(void);
	std::pair<double, double> getHddSystemUsage(void);

	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	HANDLE self;
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;

	long long lastHddSelfRead, lastHddSelfWrite, lastHddSystemRead, lastHddSystemWrite;
	milliseconds hddSelfLastMeasureTime, hddSystemLastMeasureTime;
};
#endif
#endif /* INCLUDE_WINDOWSRESPROVIDER_HPP_ */
