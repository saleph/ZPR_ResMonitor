#ifndef SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_
#define SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_

#include <vector>
#include <ctime>
#include "CpuState.hpp"
#include "RamState.hpp"
#include "HddState.hpp"

class ResUsageProvider
{
public:
	ResUsageProvider();

	virtual CpuState getCpuState(void) = 0;

	virtual RamState getRamState(void) = 0;

	virtual HddState getHddState(void) = 0;

	virtual ~ResUsageProvider();
};

#endif /* SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_ */
