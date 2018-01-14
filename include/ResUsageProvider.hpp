#ifndef SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_
#define SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_

#include <vector>
#include <ctime>
#include "CpuState.hpp"
#include "RamState.hpp"
#include "HddState.hpp"

/**
 * 	Class that defines interface for system-specific resource usage providers
 * 	(Linux and Windows resource providers).
 */
class ResUsageProvider
{
public:
	ResUsageProvider();

	/**
	 * 	@brief	Interface method that provides CpuState object
	 * 	according to current CPU state.
	 *
	 * 	@return	CpuState object with current CPU state.
	 */
	virtual CpuState getCpuState() { return CpuState(); };

	/**
	 * 	@brief	Interface method that provides RamState object
	 * 	according to current RAM memory state.
	 *
	 * 	@return	RamState object with current RAM memory state.
	 */
	virtual RamState getRamState() { return RamState(); };

	/**
	 * 	@brief	Interface method that provides HddState object
	 * 	according to current hard drive state.
	 *
	 * 	@return	HddState object with current hard drive state.
	 */
	virtual HddState getHddState() { return HddState(); };

	virtual ~ResUsageProvider();
};

#endif /* SRC_LIBRARY_SRC_RESUSAGEPROVIDER_H_ */
