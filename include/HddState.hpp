#ifndef SRC_LIBRARY_SRC_HDDSTATE_H_
#define SRC_LIBRARY_SRC_HDDSTATE_H_

#include "configrawdata.hpp"

/**
 * 	@brief	Class that represents state of the Hard Drive bandwidth.
 * 	Contains information about total bandwidth, used bandwidth and
 * 	bandwidth used by this application in MB/s.
 */
class HddState
{
public:
	HddState();
	HddState(double _hddInUseKBsRead, double _hddInUseKBsWrite,
			double _monitorHddInUseKBsRead, double _monitorHddInUseKBsWrite);
	virtual ~HddState();

	double currKBsUsedRead(void);
	double currKBsUsedWrite(void);
	double currMonitorKBsUsedRead(void);
	double currMonitorKBsUsedWrite(void);
	void setKBsUsedRead(double _hddInUseKBsRead);
	void setKBsUsedWrite(double _hddInUseKBsWrite);
	void setMonitorKBsUsedRead(double _monitorHddInUseKBsRead);
	void setMonitorKBsUsedWrite(double _monitorHddInUseKBsWrite);

	bool operator>(const TriggerType &triggerType) const;

    /// Addition operator. Used in mean computing.
    HddState &operator+=(const HddState &other) {
        hddInUseKBsRead += other.hddInUseKBsRead;
        hddInUseKBsWrite += other.hddInUseKBsWrite;
        monitorHddInUseKBsRead += other.monitorHddInUseKBsRead;
        monitorHddInUseKBsWrite += other.monitorHddInUseKBsWrite;
		return *this;
    }

    /// Divide operator. Used in mean computing.
    HddState &operator/=(double val) {
        hddInUseKBsRead /= val;
        hddInUseKBsWrite /= val;
        monitorHddInUseKBsRead /= val;
        monitorHddInUseKBsWrite /= val;
		return *this;
    }

    friend std::ostream & operator<<(std::ostream & stream, const HddState & hddState);
private:
	double hddInUseKBsRead;
	double hddInUseKBsWrite;
	double monitorHddInUseKBsRead;
	double monitorHddInUseKBsWrite;
};

std::ostream & operator<<(std::ostream & stream, const HddState & hddState);

#endif /* SRC_LIBRARY_SRC_HDDSTATE_H_ */
