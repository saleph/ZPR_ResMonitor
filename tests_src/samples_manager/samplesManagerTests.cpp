#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <LinuxResProvider.hpp>

#include "SamplingManager.hpp"

BOOST_AUTO_TEST_SUITE(samplesManagerTests)

BOOST_AUTO_TEST_CASE(first_test)
{
    std::vector<LogType> logtypes;
    logtypes.emplace_back(LogType());
    SamplingManager samplingManager{LinuxResProvider(), logtypes, std::vector<TriggerType>()};
    samplingManager.getCpuLog(LogType());
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

BOOST_AUTO_TEST_SUITE_END();