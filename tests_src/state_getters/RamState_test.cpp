#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>

#include "RamState.hpp"

BOOST_AUTO_TEST_CASE(RamState_setters_getters_test)
{
	RamState rState;

	double r = 1.0;
	rState.setMBUsed(r);
	BOOST_CHECK(rState.currMBUsed() == r);

	rState.setMonitorMBUsed(r);
	BOOST_CHECK(rState.currMonitorMBUsed() == r);

	rState.setTotalMB(r);
	BOOST_CHECK(rState.totalMB() == r);
}

BOOST_AUTO_TEST_CASE(RamState_percent_getter_test)
{
	RamState rState;

	double total = 100.0;
	double usedTotal = 12.0;
	double usedMonitor = 5.0;

	rState.setTotalMB(total);
	rState.setMBUsed(usedTotal);
	BOOST_CHECK(rState.currPercentageUsed() == usedTotal);

	rState.setMonitorMBUsed(usedMonitor);
	BOOST_CHECK(rState.currMonitorPercentageUsed() == usedMonitor);
}
