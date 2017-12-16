
#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <string>
#include <configurationParser.hpp>

BOOST_AUTO_TEST_SUITE(configurationParser)

BOOST_AUTO_TEST_CASE(first_test) {
	ConfigurationParser parser ("tests_src/conf_parser/example");
	parser.run();
	BOOST_TEST(parser.getTriggerTypes().size() == 10);
	BOOST_TEST(parser.getLogTypes().size() == 26);
}

BOOST_AUTO_TEST_SUITE_END()
