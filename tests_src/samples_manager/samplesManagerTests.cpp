#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <LinuxResProvider.hpp>
#include <ConfigurationParser.hpp>

#include "SamplingManager.hpp"

BOOST_AUTO_TEST_SUITE(samplesManagerTests)

BOOST_AUTO_TEST_CASE(first_test)
{
    std::string conf = "\n"
            "    trigger cpu over {70%, 80%, 90%} last for {20s, 1m}\n"
            "    trigger memory over 200MB last for 1s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h, 6h, 12h} resolution {1m, 5m, 15m, 30m, 1h}\n"
            "    log memory for {12h, 1d, 1d12h, 2d} resolution {1h, 6h}\n"
            "    trigger disk under 1MB/s last for 2s\n"
            "    log disk for 72h resolution {1h}\n"
            "    log disk for 72h resolution {2h}\n"
            "    log disk for 72h resolution 3h\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();
    SamplingManager samplingManager{LinuxResProvider(), parser.getLogTypes(), parser.getTriggerTypes()};
    samplingManager.pollingFunction();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

BOOST_AUTO_TEST_SUITE_END();