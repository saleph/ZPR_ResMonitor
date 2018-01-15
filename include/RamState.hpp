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
    /// Constructs new RamState object with default values set to 0.0.
	RamState();

    /// Constructs new RamState object with particular RAM usage provided with arguments.
	RamState(double _totalRamMB, double _ramInUseMB, double _monitorRamInUseMB);

    /// Destructor of the RamState object.
	virtual ~RamState();

	/// Returns current system memory usage in MB
	double currMBUsed(void);

	/// Returns current system memory usage in %
	double currPercentageUsed(void) const;

	/// Returns current memory usage by this process in MB
	double currMonitorMBUsed(void);

	/// Returns current memory usage by this process in %
	double currMonitorPercentageUsed(void) const;

	/// Return total MB of RAM memory available in this PC
	double totalMB(void);

	/// Sets current system memory used in MB
	void setMBUsed(double _ramInUseMB);

	/// Sets current memory used by this process in MB
	void setMonitorMBUsed(double _monitorRamInUseMB);

	/// Sets total system memory in MB
	void setTotalMB(double _totalRamMB);
  
  bool operator>(const TriggerType &triggerType) const;

  /// Addition operator. Used in mean computing.
  RamState &operator+=(const RamState &other) {
      ramInUseMB += other.ramInUseMB;
      monitorRamInUseMB += other.monitorRamInUseMB;
      return *this;
  }
  /// Divide operator. Used in mean computing.
  RamState &operator/=(double val) {
      ramInUseMB /= val;
      monitorRamInUseMB /= val;
      return *this;
  }

  friend std::ostream & operator<<(std::ostream & stream, const RamState & ramState);
private:
	double totalRamMB;
	double ramInUseMB;
	double monitorRamInUseMB;
};

std::ostream & operator<<(std::ostream & stream, const RamState & ramState);

#endif /* SRC_LIBRARY_SRC_RAMSTATE_H_ */
