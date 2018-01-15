#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>

#include "HddState.hpp"

BOOST_AUTO_TEST_CASE(HddState_setters_getters_test)
{
	HddState hddState;

	double m = 17.0;
	hddState.setKBsUsedRead(m);
	BOOST_CHECK(hddState.currKBsUsedRead() == m);

	hddState.setKBsUsedWrite(m);
	BOOST_CHECK(hddState.currKBsUsedWrite() == m);

	hddState.setMonitorKBsUsedRead(m);
	BOOST_CHECK(hddState.currMonitorKBsUsedRead() == m);

	hddState.setMonitorKBsUsedWrite(m);
	BOOST_CHECK(hddState.currMonitorKBsUsedWrite() == m);
}
