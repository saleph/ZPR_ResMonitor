#ifndef INCLUDE_LINUXRESPROVIDER_HPP_
#define INCLUDE_LINUXRESPROVIDER_HPP_

#ifdef __linux__

#include "ResUsageProvider.hpp"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "sys/times.h"
#include "sys/vtimes.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>
#include <utility>
#include <chrono>

using namespace std::chrono;

/**
 * 	Class that provides states of resources in Linux system
 * 	(CPU, RAM and Hard Drive). Can be only used on Linux systems.
 * 	Corresponding resource provider class for windows is WindowsResProvider.
 */
class LinuxResProvider : public ResUsageProvider
{
public:
	/**
	 *	@brief	Default constructor for LinuxResProvider.
	 *	Initializes first CPU and Hdd usage measurements.
	 */
	LinuxResProvider();

	/**
	 * 	@brief	Method that provides CpuState object in Linux system
	 * 	according to current CPU state.
	 *
	 * 	@return	CpuState object with current CPU usage state.
	 */
	CpuState getCpuState(void);

	/**
	 * 	@brief	Method that provides RamState object in Linux system
	 * 	according to current RAM memory state.
	 *
	 * 	@return	RamState object with current RAM memory usage state.
	 */
	RamState getRamState(void);

	/**
	 * 	@brief	Method that provides HddState object in Linux system
	 * 	according to current hard drive bandwidth usage state.
	 *
	 * 	@return	HddState object with current hard drive bandwidth usage state.
	 */
	HddState getHddState(void);

private:
	int parseLineRam(char* line);

	void initSystemCpuUsage(void);
	double getSystemCpuUsage(void);
	void initSelfCpuUsage(void);
	double getSelfCpuUsage(void);
	void initHddUsage(void);
	std::pair<double, double> getHddSelfUsage(void);
	std::pair<double, double> getHddSystemUsage(void);
	int getRamSelfUsage(void);	// Value in KB!

	unsigned long long lastTotalUser, lastTotalNiced, lastTotalSys, lastTotalIdle;
	unsigned long long lastHddSelfRead, lastHddSelfWrite, lastHddSystemRead, lastHddSystemWrite;
	clock_t lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	unsigned hddSectorSize;

	milliseconds hddSelfLastMeasureTime, hddSystemLastMeasureTime;

};
#endif /* __linux__ */
#endif /* INCLUDE_LINUXRESPROVIDER_HPP_ */
