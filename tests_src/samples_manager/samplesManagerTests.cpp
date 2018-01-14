#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <LinuxResProvider.hpp>
#include <ConfigurationParser.hpp>

#include "SamplingManager.hpp"
#include <memory>


BOOST_AUTO_TEST_SUITE(samplesManagerTests)

class ResProviderMock : public ResUsageProvider {
public:
    double CPU_USE = 85.0;
    double RAM_USE = 500.0;
    double HDD_USE = 100.0;
    ResProviderMock() = default;

    virtual ~ResProviderMock() = default;

    CpuState getCpuState() override {
        CpuState state(CPU_USE, 10.0);
        return state;
    };

    RamState getRamState() override {
        RamState state(1000.0, RAM_USE, 100.0);
        return state;

    };

    HddState getHddState() override {
        HddState state(HDD_USE, HDD_USE, 2000, 2000);
        return state;

    };

};

class SamplerManagerMock : public SamplingManager {
    const long SAMPLES_NUMBER = 10;
public:

    SamplerManagerMock(std::shared_ptr<ResUsageProvider> &resUsageProvider,
    const std::vector<LogType> &logTypes,
    const std::vector<TriggerType> &triggerTypes,
            std::function<void(const TriggerType &)> triggerCallback = [](const TriggerType &) { })
    : SamplingManager(resUsageProvider, logTypes, triggerTypes, std::move(triggerCallback))
    {
        SamplingManager::SAMPLING_TIME_INTERVAL_MS = 0;
        SamplingManager::afterPollCallback = [this](long samplesNumber) {
            if (samplesNumber >= SAMPLES_NUMBER)
                SamplingManager::isSampling = false;
        };
    };

    void waitForEndOfSampling() {
        SamplingManager::pollerThread.join();
    }
};

BOOST_AUTO_TEST_CASE(log_test)
{
    std::string conf = "\n"
            "    trigger cpu over {70%, 80%, 90%} last for {8s, 1m}\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int cpuTriggers{0};
    std::atomic_int ramTriggers{0};
    std::atomic_int hddTriggers{0};
    auto callback = [&cpuTriggers, &ramTriggers, &hddTriggers](const TriggerType &trigger) {
        if (trigger.resource == TriggerType::Resource::CPU)
            ++cpuTriggers;
        if (trigger.resource == TriggerType::Resource::MEMORY)
            ++ramTriggers;
        if (trigger.resource == TriggerType::Resource::DISK)
            ++hddTriggers;
    };

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes(), callback
            );
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    for (auto &&log : parser.getLogTypes()) {
        if (log.resource == LogType::Resource::CPU)
            BOOST_CHECK_EQUAL(samplingManager->getCpuLog(log)->size(), 1);

        if (log.resource == LogType::Resource::MEMORY)
            BOOST_CHECK_EQUAL(samplingManager->getRamLog(log)->size(), 0);

        if (log.resource == LogType::Resource::DISK)
            BOOST_CHECK_EQUAL(samplingManager->getHddLog(log)->size(), 2);
    }
}

BOOST_AUTO_TEST_CASE(trigger_test)
{
    std::string conf = "\n"
            "    trigger cpu over {70%, 80%, 90%} last for {8s, 1m}\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int cpuTriggers{0};
    std::atomic_int ramTriggers{0};
    std::atomic_int hddTriggers{0};
    auto callback = [&cpuTriggers, &ramTriggers, &hddTriggers](const TriggerType &trigger) {
        if (trigger.resource == TriggerType::Resource::CPU)
            ++cpuTriggers;
        if (trigger.resource == TriggerType::Resource::MEMORY)
            ++ramTriggers;
        if (trigger.resource == TriggerType::Resource::DISK)
            ++hddTriggers;
    };

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes(), callback
            );
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // trigger cpu over {70%, 80%, 90%} last for {8s, 1m}
    // ResUsageMock value: 85%, so 2 triggers should occur in 10 samples
    BOOST_CHECK_EQUAL(cpuTriggers, 2);
    // trigger memory over 200MB last for 6s
    // ResUsageMock value: 500MB, so 1 triggers should occur in 10 samples
    BOOST_CHECK_EQUAL(ramTriggers, 1);
    // trigger disk under 10MB/s last for 2s
    // ResUsageMock value: 0.1MB/s, so 1 triggers should occur in 10 samples (trigger fires only once!)
    BOOST_CHECK_EQUAL(hddTriggers, 1);
}

BOOST_AUTO_TEST_SUITE_END();