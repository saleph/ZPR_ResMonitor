/*
 * CpuMonitor.h
 *
 *  Created on: Dec 7, 2017
 *      Author: piotr
 */

#ifndef SRC_LIBRARY_SRC_CPUMONITOR_H_
#define SRC_LIBRARY_SRC_CPUMONITOR_H_

#include <vector>
#include <ctime>


class CpuMonitor
{
public:
	CpuMonitor();

	virtual bool getCpuUsage(std::vector<int> & cpuUsage, std::time_t period) = 0;

	virtual bool getCpuUsage(std::time_t period) = 0;

	virtual ~CpuMonitor();
};

#endif /* SRC_LIBRARY_SRC_CPUMONITOR_H_ */
