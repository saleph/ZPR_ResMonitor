#ifndef SRC_LIBRARY_SRC_HDDSTATE_H_
#define SRC_LIBRARY_SRC_HDDSTATE_H_

#include "configrawdata.hpp"

/**
 * 	@brief	Class that represents state of the Hard Drive bandwidth.
 * 	Contains information about hdd bandwidth used by system and
 * 	bandwidth used by this application in KB/s.
 */
class HddState
{
public:
	/// Construct new HddState object with all usage values set to 0.0.
	HddState();

    /// Constructs new HddState object with particular Hdd usage provided with arguments.
	HddState(double _hddInUseKBsRead, double _hddInUseKBsWrite,
			double _monitorHddInUseKBsRead, double _monitorHddInUseKBsWrite);

    /// Destructor of the HddState object.
	virtual ~HddState();

	/// Returns current system hdd read bandwidth usage in KB/s
	double currKBsUsedRead(void);

	/// Returns current system hdd write bandwidth usage in KB/s
	double currKBsUsedWrite(void);

	/// Returns current hdd read bandwidth usage for this process in KB/s
	double currMonitorKBsUsedRead(void);

	/// Returns current hdd write bandwidth usage for this process in KB/s
	double currMonitorKBsUsedWrite(void);

	/// Sets current hdd read bandwidth usage for the system in KB/s
	void setKBsUsedRead(double _hddInUseKBsRead);

	/// Sets current hdd write bandwidth usage for the system in KB/s
	void setKBsUsedWrite(double _hddInUseKBsWrite);

	/// Sets current hdd read bandwidth usage for this process in KB/s
	void setMonitorKBsUsedRead(double _monitorHddInUseKBsRead);

	/// Sets current hdd write bandwidth usage for this process in KB/s
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
