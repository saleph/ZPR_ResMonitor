#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>
#include <ConfigurationParser.hpp>
#include "SamplingManager.hpp"
#include "PredicateEngine.hpp"
#include "Predicate.hpp"
#include "PredicateTypes.hpp"
#include <memory>
#include <boost/log/trivial.hpp>


BOOST_AUTO_TEST_SUITE(predicateEngineTests)

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


BOOST_AUTO_TEST_CASE(predicate_1or2and3_both_or_active) {
        std::string conf = "\n"
                "    trigger cpu over 80% last for 8s\n"
                "    trigger memory over 200MB last for 6s\n"
                "    trigger disk under 10MB/s last for 2s\n"
                "    trigger memory over 75% last for 10s\n"
                "    trigger disk over 9mb/s last for 1m2s\n"
                "\n"
                "    log cpu for {3h} resolution {7s}\n"
                "    log memory for {12h} resolution {1h}\n"
                "    log disk for 3h resolution 4s\n";
        std::stringstream stream(conf);
        ConfigurationParser parser (stream);
        parser.run();

        std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

        std::atomic_int predicateFiredNumber{0};

        auto callback = [&predicateFiredNumber](){
            ++predicateFiredNumber;
        };

        std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1or2and3>(
                callback,
                parser.getTriggerTypes()[0], // trigger cpu over 80% last for 8s
                parser.getTriggerTypes()[1], // trigger memory over 200MB last for 6s
                parser.getTriggerTypes()[2]  // trigger disk under 10MB/s last for 2s
        );

        std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
        engine->addPredicate(predicate);

        // samplerManagerMock is set to take only 10 samples
        // (as 10 samples in 10 second, but without any delay between them)
        std::unique_ptr<SamplerManagerMock> samplingManager =
                std::make_unique<SamplerManagerMock>(
                        providerMock, parser.getLogTypes(), parser.getTriggerTypes()
                );
        samplingManager->connectPredicateEngine(*engine);
        samplingManager->startSampling();
        samplingManager->waitForEndOfSampling();

        // predicate should fire only once. This trigger
        // trigger memory over 200MB last for 6s
        // fire as second after
        // trigger disk under 10MB/s last for 2s
        BOOST_CHECK_EQUAL(predicateFiredNumber, 1);
}

BOOST_AUTO_TEST_CASE(predicate_1or2and3_one_or_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1or2and3>(
            callback,
            parser.getTriggerTypes()[0], // trigger cpu over 80% last for 8s
            parser.getTriggerTypes()[3], // trigger memory over 75% last for 10s
            parser.getTriggerTypes()[2]  // trigger disk under 10MB/s last for 2s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should fire only once.
    // trigger disk under 10MB/s last for 2s => trigger cpu over 80% last for 8s
    BOOST_CHECK_EQUAL(predicateFiredNumber, 1);
}

BOOST_AUTO_TEST_CASE(predicate_1or2and3_none_or_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1or2and3>(
            callback,
            parser.getTriggerTypes()[4], // trigger disk over 9mb/s last for 1m2s
            parser.getTriggerTypes()[3], // trigger memory over 75% last for 10s
            parser.getTriggerTypes()[2]  // trigger disk under 10MB/s last for 2s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 0);
}


BOOST_AUTO_TEST_CASE(predicate_1and2_both_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1and2>(
            callback,
            parser.getTriggerTypes()[0], // trigger cpu over 80% last for 8s
            parser.getTriggerTypes()[1]  // trigger memory over 200MB last for 6s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 1);
}

BOOST_AUTO_TEST_CASE(predicate_1and2_one_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1and2>(
            callback,
            parser.getTriggerTypes()[0], // trigger cpu over 80% last for 8s
            parser.getTriggerTypes()[3]  // trigger memory over 75% last for 10s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 0);
}

BOOST_AUTO_TEST_CASE(predicate_1and2_none_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1and2>(
            callback,
            parser.getTriggerTypes()[4], // trigger disk over 9mb/s last for 1m2s
            parser.getTriggerTypes()[3]  // trigger memory over 75% last for 10s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 0);
}

BOOST_AUTO_TEST_CASE(predicate_1operator_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1element>(
            callback,
            parser.getTriggerTypes()[0] // trigger cpu over 80% last for 8s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 1);
}


BOOST_AUTO_TEST_CASE(predicate_1operator_not_active) {
    std::string conf = "\n"
            "    trigger cpu over 80% last for 8s\n"
            "    trigger memory over 200MB last for 6s\n"
            "    trigger disk under 10MB/s last for 2s\n"
            "    trigger memory over 75% last for 10s\n"
            "    trigger disk over 9mb/s last for 1m2s\n"
            "\n"
            "    log cpu for {3h} resolution {7s}\n"
            "    log memory for {12h} resolution {1h}\n"
            "    log disk for 3h resolution 4s\n";
    std::stringstream stream(conf);
    ConfigurationParser parser (stream);
    parser.run();

    std::shared_ptr<ResUsageProvider> providerMock = std::make_shared<ResProviderMock>();

    std::atomic_int predicateFiredNumber{0};

    auto callback = [&predicateFiredNumber](){
        ++predicateFiredNumber;
    };

    std::shared_ptr<Predicate> predicate = std::make_shared<Predicate_1element>(
            callback,
            parser.getTriggerTypes()[3]  // trigger memory over 75% last for 10s
    );

    std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();
    engine->addPredicate(predicate);

    // samplerManagerMock is set to take only 10 samples
    // (as 10 samples in 10 second, but without any delay between them)
    std::unique_ptr<SamplerManagerMock> samplingManager =
            std::make_unique<SamplerManagerMock>(
                    providerMock, parser.getLogTypes(), parser.getTriggerTypes()
            );
    samplingManager->connectPredicateEngine(*engine);
    samplingManager->startSampling();
    samplingManager->waitForEndOfSampling();

    // predicate should not fire.
    BOOST_CHECK_EQUAL(predicateFiredNumber, 0);
}

BOOST_AUTO_TEST_SUITE_END();