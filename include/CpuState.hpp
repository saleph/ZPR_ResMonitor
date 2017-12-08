/*
 * CpuState.h
 *
 *  Created on: Dec 8, 2017
 *      Author: piotr
 */

#ifndef SRC_LIBRARY_SRC_CPUSTATE_H_
#define SRC_LIBRARY_SRC_CPUSTATE_H_

class CpuState
{
public:
	CpuState();
	CpuState(double _cpuInUsePercentage, double _monitorCpuInUsePercentage);
	virtual ~CpuState();

	double currPercentageUsed(void);
	double currMonitorPercentageUsed(void);
	void setPercentageUsed(double _cpuInUsePercentage);
	void setMonitorPercentageUsed(double _monitorCpuInUsePercentage);

private:
	double cpuInUsePercentage;
	double monitorCpuInUsePercentage;
};

#endif /* SRC_LIBRARY_SRC_CPUSTATE_H_ */
