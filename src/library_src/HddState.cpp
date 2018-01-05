#include "HddState.hpp"

HddState::HddState()
{
	totalHddMBs = 0.0;
	hddInUseMBs = 0.0;
	monitorHddInUseMBs = 0.0;
}

HddState::HddState(double _totalHddMBs, double _hddInUseMBs, double _monitorHddInUseMBs)
{
	totalHddMBs = _totalHddMBs;
	hddInUseMBs = _hddInUseMBs;
	monitorHddInUseMBs = _monitorHddInUseMBs;
}

HddState::~HddState(){}

double HddState::currMBsUsed(void)
{
	return hddInUseMBs;
}

double HddState::currPercentageUsed(void)
{
	if(totalHddMBs != 0.0)
		return hddInUseMBs/totalHddMBs*100.0;
	return -1.0;
}

double HddState::currMonitorMBsUsed(void)
{
	return monitorHddInUseMBs;
}

double HddState::currMonitorPercentageUsed(void)
{
	if(totalHddMBs != 0.0)
		return monitorHddInUseMBs/totalHddMBs*100.0;
	return -1.0;
}

double HddState::totalMBs(void)
{
	return totalHddMBs;
}

void HddState::setMBsUsed(double _hddInUseMBs)
{
	hddInUseMBs = _hddInUseMBs;
}

void HddState::setMonitorMBsUsed(double _monitorHddInUseMBs)
{
	monitorHddInUseMBs = _monitorHddInUseMBs;
}

void HddState::setTotalMBs(double _totalHddMBs)
{
	totalHddMBs = _totalHddMBs;
}
