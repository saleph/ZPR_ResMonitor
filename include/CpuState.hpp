#ifndef SRC_LIBRARY_SRC_CPUSTATE_H_
#define SRC_LIBRARY_SRC_CPUSTATE_H_

#include "configrawdata.hpp"

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

	bool operator>(const TriggerType &triggerType) const;

    /// Addition operator. Used in mean computing.
    CpuState &operator+=(const CpuState &other) {
        cpuInUsePercentage += other.cpuInUsePercentage;
        monitorCpuInUsePercentage += other.monitorCpuInUsePercentage;
		return *this;
    }
    /// Divide operator. Used in mean computing.
    CpuState &operator/=(double val) {
        cpuInUsePercentage /= val;
        monitorCpuInUsePercentage /= val;
		return *this;
    }

    friend std::ostream & operator<<(std::ostream & stream, const CpuState & cpuState);
private:
	double cpuInUsePercentage;
	double monitorCpuInUsePercentage;
};

std::ostream & operator<<(std::ostream & stream, const CpuState & cpuState);

#endif /* SRC_LIBRARY_SRC_CPUSTATE_H_ */
