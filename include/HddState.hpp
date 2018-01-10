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

private:
	double hddInUseKBsRead;
	double hddInUseKBsWrite;
	double monitorHddInUseKBsRead;
	double monitorHddInUseKBsWrite;
};

#endif /* SRC_LIBRARY_SRC_HDDSTATE_H_ */
