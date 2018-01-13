#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>

#include "CpuState.hpp"

BOOST_AUTO_TEST_CASE(CpuState_setters_getters_test)
{
	CpuState cpuState;

	double m = 8.9;
	cpuState.setPercentageUsed(m);
	BOOST_CHECK(cpuState.currPercentageUsed() == m);

	cpuState.setMonitorPercentageUsed(m);
	BOOST_CHECK(cpuState.currMonitorPercentageUsed() == m);
}

