/*
 * RamState.cpp
 *
 *  Created on: Dec 8, 2017
 *      Author: piotr
 */

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


void RamState::init(double _totalRamMB)
{
	totalRamMB = _totalRamMB;
}

double RamState::currMBUsed(void)
{
	return ramInUseMB;
}

double RamState::currPercentageUsed(void)
{
	if(totalRamMB != 0.0)
		return ramInUseMB/totalRamMB;
	return -1.0;
}

double RamState::currMonitorMBUsed(void)
{
	return monitorRamInUseMB;
}

double RamState::currMonitorPercentageUsed(void)
{
	if(totalRamMB != 0.0)
		return monitorRamInUseMB/totalRamMB;
	return -1.0;
}

void RamState::setMBUsed(double _ramInUseMB)
{
	ramInUseMB = _ramInUseMB;
}

void RamState::setMonitorMBUsed(double _monitorRamInUseMB)
{
	monitorRamInUseMB = _monitorRamInUseMB;
}
