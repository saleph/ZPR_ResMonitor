#include "HddState.hpp"

HddState::HddState()
{
	hddInUseKBsRead = 0.0;
	hddInUseKBsWrite = 0.0;
	monitorHddInUseKBsRead = 0.0;
	monitorHddInUseKBsWrite = 0.0;
}

HddState::HddState(double _hddInUseKBsRead, double _hddInUseKBsWrite,
		double _monitorHddInUseKBsRead, double _monitorHddInUseKBsWrite)
{
	hddInUseKBsRead = _hddInUseKBsRead;
	hddInUseKBsWrite = _hddInUseKBsWrite;
	monitorHddInUseKBsRead = _monitorHddInUseKBsRead;
	monitorHddInUseKBsWrite = _monitorHddInUseKBsWrite;
}

HddState::~HddState(){}

double HddState::currKBsUsedRead(void)
{
	return hddInUseKBsRead;
}

double HddState::currKBsUsedWrite(void)
{
	return hddInUseKBsWrite;
}

double HddState::currMonitorKBsUsedRead(void)
{
	return monitorHddInUseKBsRead;
}

double HddState::currMonitorKBsUsedWrite(void)
{
	return monitorHddInUseKBsWrite;
}

void HddState::setKBsUsedRead(double _hddInUseKBsRead)
{
	hddInUseKBsRead = _hddInUseKBsRead;
}

void HddState::setKBsUsedWrite(double _hddInUseKBsWrite)
{
	hddInUseKBsWrite = _hddInUseKBsWrite;
}

void HddState::setMonitorKBsUsedRead(double _monitorHddInUseKBsRead)
{
	monitorHddInUseKBsRead = _monitorHddInUseKBsRead;
}

void HddState::setMonitorKBsUsedWrite(double _monitorHddInUseKBsWrite)
{
	monitorHddInUseKBsWrite = _monitorHddInUseKBsWrite;
}

