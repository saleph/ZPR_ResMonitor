#ifndef SRC_LIBRARY_SRC_RAMSTATE_H_
#define SRC_LIBRARY_SRC_RAMSTATE_H_

class RamState
{
public:
	RamState();
	RamState(double _totalRamMB, double _ramInUseMB, double _monitorRamInUseMB);
	virtual ~RamState();

	void init(double _totalRamMB);
	double currMBUsed(void);
	double currPercentageUsed(void);
	double currMonitorMBUsed(void);
	double currMonitorPercentageUsed(void);
	void setMBUsed(double _ramInUseMB);
	void setMonitorMBUsed(double _monitorRamInUseMB);

private:
	double totalRamMB;
	double ramInUseMB;
	double monitorRamInUseMB;
};

#endif /* SRC_LIBRARY_SRC_RAMSTATE_H_ */
