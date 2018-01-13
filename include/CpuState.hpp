#ifndef SRC_LIBRARY_SRC_CPUSTATE_H_
#define SRC_LIBRARY_SRC_CPUSTATE_H_

/**
 * 	@brief	Class that represents state of the CPU usage.
 * 	Contains information about percentage of CPU usage for
 * 	system and this application.
 */
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
