#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <LinuxResProvider.hpp>
#include <configurationParser.hpp>

#include "SamplingManager.hpp"

BOOST_AUTO_TEST_SUITE(samplesManagerTests)

BOOST_AUTO_TEST_CASE(first_test)
{
    ConfigurationParser parser ("../tests_src/conf_parser/example");
    parser.run();
    SamplingManager samplingManager{LinuxResProvider(), parser.getLogTypes(), parser.getTriggerTypes()};
    samplingManager.pollingFunction();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

BOOST_AUTO_TEST_SUITE_END();