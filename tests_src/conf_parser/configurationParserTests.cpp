#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <string>
#include <ConfigurationParser.hpp>

BOOST_AUTO_TEST_SUITE(configurationParser)

BOOST_AUTO_TEST_CASE(first_test) {
<<<<<<< HEAD
	ConfigurationParser parser("tests_src/conf_parser/example");
||||||| merged common ancestors
	ConfigurationParser parser ("tests_src/conf_parser/example");
=======
	ConfigurationParser parser ("../tests_src/conf_parser/example");
>>>>>>> dev
	parser.run();
	BOOST_TEST(parser.getTriggerTypes().size() == 10);
	BOOST_TEST(parser.getLogTypes().size() == 26);
}

BOOST_AUTO_TEST_SUITE_END()