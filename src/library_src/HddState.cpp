/*
 * HddState.cpp
 *
 *  Created on: Dec 8, 2017
 *      Author: piotr
 */

#include "HddState.hpp"

double HddState::totalHddMBs = 0.0;

HddState::HddState()
{
	hddInUseMBs = 0.0;
	monitorHddInUseMBs = 0.0;
}

HddState::HddState(double _hddInUseMBs, double _monitorHddInUseMBs)
{
	hddInUseMBs = _hddInUseMBs;
	monitorHddInUseMBs = _monitorHddInUseMBs;
}

HddState::~HddState(){}


void HddState::init(double _totalHddMBs)
{
	totalHddMBs = _totalHddMBs;
}

double HddState::currMBsUsed(void)
{
	return hddInUseMBs;
}

double HddState::currPercentageUsed(void)
{
	if(totalHddMBs != 0.0)
		return hddInUseMBs/totalHddMBs;
	return -1.0;
}

double HddState::currMonitorMBsUsed(void)
{
	return monitorHddInUseMBs;
}

double HddState::currMonitorPercentageUsed(void)
{
	if(totalHddMBs != 0.0)
		return monitorHddInUseMBs/totalHddMBs;
	return -1.0;
}

void HddState::setMBsUsed(double _hddInUseMBs)
{
	hddInUseMBs = _hddInUseMBs;
}

void HddState::setMonitorMBsUsed(double _monitorHddInUseMBs)
{
	monitorHddInUseMBs = _monitorHddInUseMBs;
}
