#include "CpuState.hpp"

CpuState::CpuState()
{
	cpuInUsePercentage = 0.0;
	monitorCpuInUsePercentage = 0.0;
}

CpuState::~CpuState(){}

CpuState::CpuState(double _cpuInUsePercentage, double _monitorCpuInUsePercentage)
{
	cpuInUsePercentage = _cpuInUsePercentage;
	monitorCpuInUsePercentage = _monitorCpuInUsePercentage;
}

double CpuState::currPercentageUsed(void)
{
	return cpuInUsePercentage;
}

double CpuState::currMonitorPercentageUsed(void)
{
	return monitorCpuInUsePercentage;
}

void CpuState::setPercentageUsed(double _cpuInUsePercentage)
{
	cpuInUsePercentage = _cpuInUsePercentage;
}

void CpuState::setMonitorPercentageUsed(double _monitorCpuInUsePercentage)
{
	monitorCpuInUsePercentage = _monitorCpuInUsePercentage;
}
