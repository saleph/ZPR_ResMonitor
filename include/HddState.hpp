#ifndef SRC_LIBRARY_SRC_HDDSTATE_H_
#define SRC_LIBRARY_SRC_HDDSTATE_H_

/**
 * 	@brief	Class that represents state of the Hard Drive bandwidth.
 * 	Contains information about total bandwidth, used bandwidth and
 * 	bandwidth used by this application in MB/s.
 */
class HddState
{
public:
	HddState();
	HddState(double _totalHddMBs, double _hddInUseMBs, double _monitorHddInUseMBs);
	virtual ~HddState();

	double currMBsUsed(void);
	double currPercentageUsed(void);
	double currMonitorMBsUsed(void);
	double currMonitorPercentageUsed(void);
	double totalMBs(void);
	void setMBsUsed(double _ramInUseMB);
	void setMonitorMBsUsed(double _monitorRamInUseMB);
	void setTotalMBs(double _totalHddMBs);

private:
	double totalHddMBs;
	double hddInUseMBs;
	double monitorHddInUseMBs;
};

#endif /* SRC_LIBRARY_SRC_HDDSTATE_H_ */
