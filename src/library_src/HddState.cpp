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

bool HddState::operator>(const TriggerType &triggerType) const {
	// for disk percent is not supported
	assert(triggerType.triggerValue.unitType != ResourceValue::ResourceUnit::PERCENT);

	bool out = false;
    double maxInUse = std::max(hddInUseKBsRead, hddInUseKBsWrite);

	if (triggerType.triggerValue.unitType == ResourceValue::ResourceUnit::MB) {
		if (triggerType.fluctuationType == TriggerType::FluctuationType::OVER)
			out = maxInUse/1000 > triggerType.triggerValue.value;
		else
			out = maxInUse/1000 < triggerType.triggerValue.value;
	} else if (triggerType.triggerValue.unitType == ResourceValue::ResourceUnit::GB) {
		if (triggerType.fluctuationType == TriggerType::FluctuationType::OVER)
			out = maxInUse/1000000 > triggerType.triggerValue.value;
		else
			out = maxInUse/1000000 < triggerType.triggerValue.value;
	}

	return out;
}

std::ostream & operator<<(std::ostream & stream, const HddState & hddState)
{
	stream << "Hdd (system) read/write [KB/s]: "
			<< hddState.hddInUseKBsRead << "/" << hddState.hddInUseKBsWrite
			<< ", Hdd (monitor) read/write KB/s]: "
			<< hddState.monitorHddInUseKBsRead << "/" << hddState.monitorHddInUseKBsWrite;
	return stream;
}
