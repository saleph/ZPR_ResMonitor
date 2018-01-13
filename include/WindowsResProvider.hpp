#ifndef INCLUDE_WINDOWSRESPROVIDER_HPP_
#define INCLUDE_WINDOWSRESPROVIDER_HPP_


#ifdef _WIN32

#include "ResUsageProvider.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>
#include <utility>
#include <chrono>
#include "windows.h"
#include "psapi.h"
#include "TCHAR.h"
#include "pdh.h"

using namespace std::chrono;

class WindowsResProvider : public ResUsageProvider
{
public:
	WindowsResProvider();
	CpuState getCpuState(void);
	RamState getRamState(void);
	HddState getHddState(void);
	std::pair<double, double> getHddSelfUsage(void);
	std::pair<double, double> getHddSystemUsage(void);

private:
	void initHddUsage(void);

	int parseLineRam(char* line);
	int getRamSelfUsage(void);	// Value in KB!

	void initSystemCpuUsage(void);
	double getSystemCpuUsage(void);
	void initSelfCpuUsage(void);
	double getSelfCpuUsage(void);

	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	HANDLE self;
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
};
#endif
#endif /* INCLUDE_WINDOWSRESPROVIDER_HPP_ */
