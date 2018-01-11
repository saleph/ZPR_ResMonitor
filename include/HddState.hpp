#ifndef SRC_LIBRARY_SRC_HDDSTATE_H_
#define SRC_LIBRARY_SRC_HDDSTATE_H_

class HddState
{
public:
	HddState();
	HddState(double _totalHddMBs, double _hddInUseMBs, double _monitorHddInUseMBs);
	virtual ~HddState();

	void init(double _totalHddMBs);
	double currMBsUsed(void);
	double currPercentageUsed(void);
	double currMonitorMBsUsed(void);
	double currMonitorPercentageUsed(void);
	void setMBsUsed(double _ramInUseMB);
	void setMonitorMBsUsed(double _monitorRamInUseMB);

private:
	double totalHddMBs;
	double hddInUseMBs;
	double monitorHddInUseMBs;
};

#endif /* SRC_LIBRARY_SRC_HDDSTATE_H_ */
