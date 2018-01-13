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

bool CpuState::operator>(const TriggerType &triggerType) const {
	// for cpu only percents have sens
	assert(triggerType.triggerValue.unitType == ResourceValue::ResourceUnit::PERCENT);
	bool out;
	if (triggerType.fluctuationType == TriggerType::FluctuationType::OVER)
		out = cpuInUsePercentage > triggerType.triggerValue.value;
	else
		out = cpuInUsePercentage < triggerType.triggerValue.value;

	return out;
}