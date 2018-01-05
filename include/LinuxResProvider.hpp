#ifndef INCLUDE_LINUXRESPROVIDER_HPP_
#define INCLUDE_LINUXRESPROVIDER_HPP_

#include "ResUsageProvider.hpp"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "sys/times.h"
#include "sys/vtimes.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

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
	int parseLine(char* line);
	int getRamSelfUsage(void);	// Value in KB!

	void initSystemCpuUsage(void);
	double getSystemCpuUsage(void);
	void initSelfCpuUsage(void);
	double getSelfCpuUsage(void);

	unsigned long long lastTotalUser, lastTotalNiced, lastTotalSys, lastTotalIdle;
	clock_t lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;

};

#endif /* INCLUDE_LINUXRESPROVIDER_HPP_ */
