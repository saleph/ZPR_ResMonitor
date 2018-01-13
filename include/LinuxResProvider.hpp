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
 * 	(CPU, RAM and Hard Drive).
 */
class LinuxResProvider : public ResUsageProvider
{
public:
	LinuxResProvider();
	CpuState getCpuState(void);
	RamState getRamState(void);
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
