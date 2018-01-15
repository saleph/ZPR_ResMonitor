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
	/// Construct new CpuState object with all usage values set to 0.0.
	CpuState();

    /// Constructs new CpuState object with particular CPU usage given as arguments.
	CpuState(double _cpuInUsePercentage, double _monitorCpuInUsePercentage);

	/// Destructor of the CpuState object.
	virtual ~CpuState();

	/// Returns percentage of CPU used by the system.
	double currPercentageUsed(void);

	/// Returns percentage of CPU used by this process.
	double currMonitorPercentageUsed(void);

	/// Sets percentage of CPU used by system.
	void setPercentageUsed(double _cpuInUsePercentage);

	/// Sets percentage of CPU used by this process.
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
