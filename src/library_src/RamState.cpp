#include "RamState.hpp"

RamState::RamState()
{
	totalRamMB = 0.0;
	ramInUseMB = 0.0;
	monitorRamInUseMB = 0.0;
}

RamState::RamState(double _totalRamMB, double _ramInUseMB, double _monitorRamInUseMB)
{
	totalRamMB = _totalRamMB;
	ramInUseMB = _ramInUseMB;
	monitorRamInUseMB = _monitorRamInUseMB;
}

RamState::~RamState(){}

double RamState::currMBUsed(void)
{
	return ramInUseMB;
}

double RamState::currPercentageUsed(void)
{
	if(totalRamMB != 0.0)
		return ramInUseMB/totalRamMB * 100.0;
	return -1.0;
}

double RamState::currMonitorMBUsed(void)
{
	return monitorRamInUseMB;
}

double RamState::currMonitorPercentageUsed(void)
{
	if(totalRamMB != 0.0)
		return monitorRamInUseMB/totalRamMB*100.0;
	return -1.0;
}

double RamState::totalMB(void)
{
	return totalRamMB;
}

void RamState::setMBUsed(double _ramInUseMB)
{
	ramInUseMB = _ramInUseMB;
}

void RamState::setMonitorMBUsed(double _monitorRamInUseMB)
{
	monitorRamInUseMB = _monitorRamInUseMB;
}

void RamState::setTotalMB(double _totalRamMB)
{
	totalRamMB = _totalRamMB;
}
