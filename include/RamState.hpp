#ifndef SRC_LIBRARY_SRC_RAMSTATE_H_
#define SRC_LIBRARY_SRC_RAMSTATE_H_

#include "configrawdata.hpp"

/**
 * 	@brief	Class RamState represents state of the RAM memory.
 * 	Contains information about total, used and used by this
 * 	application RAM bytes in [MB].
 */
class RamState
{
public:
	RamState();
	RamState(double _totalRamMB, double _ramInUseMB, double _monitorRamInUseMB);
	virtual ~RamState();

	double currMBUsed(void);
	double currPercentageUsed(void) const;
	double currMonitorMBUsed(void);
	double currMonitorPercentageUsed(void);
	double totalMB(void);
	void setMBUsed(double _ramInUseMB);
	void setMonitorMBUsed(double _monitorRamInUseMB);
	void setTotalMB(double _totalRamMB);

    bool operator>(const TriggerType &triggerType) const;

    /// Addition operator. Used in mean computing.
    RamState &operator+=(const RamState &other) {
        ramInUseMB += other.ramInUseMB;
        monitorRamInUseMB += other.monitorRamInUseMB;
    }
    /// Divide operator. Used in mean computing.
    RamState &operator/=(double val) {
        ramInUseMB /= val;
        monitorRamInUseMB /= val;
    }
private:
	double totalRamMB;
	double ramInUseMB;
	double monitorRamInUseMB;
};

#endif /* SRC_LIBRARY_SRC_RAMSTATE_H_ */
