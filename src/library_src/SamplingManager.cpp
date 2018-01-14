#include "SamplingManager.hpp"

#include <utility>

SamplingManager::~SamplingManager() {
    stopSampling();
    BOOST_LOG_TRIVIAL(debug) << "joined";
}

SamplingManager::SamplingManager(std::shared_ptr<ResUsageProvider> &resUsageProvider,
                                 const std::vector<LogType> &logTypes,
                                 const std::vector<TriggerType> &triggerTypes,
                                 std::function<void(const TriggerType &)> triggerActivate,
                                 std::function<void(const TriggerType &)> triggerDeactivate)
        : isSampling(false),
          resUsageProvider(resUsageProvider),
          triggerActivate(std::move(triggerActivate)),
          triggerDeactivate(std::move(triggerDeactivate)){
    initializeSamplesBuffers(logTypes);
    initializeLoggingBuffers(logTypes);
    initializeTriggers(triggerTypes);
}

void SamplingManager::stopSampling() {
    if (!isSampling)
        return;
    isSampling = false;
    pollerThread.join();
}

void SamplingManager::startSampling() {
    if (isSampling)
        return;
    isSampling = true;
    pollerThread = std::thread(&SamplingManager::pollingFunction, this);
}

void SamplingManager::initializeSamplesBuffers(const std::vector<LogType> &logTypes) {
    long cpuMaxResolution = 1;
    long ramMaxResolution = 1;
    long hddMaxResolution = 1;
    for (auto &&log : logTypes) {
        if (log.resource == LogType::Resource::CPU) {
            cpuMaxResolution = std::max(cpuMaxResolution, log.resolution);
        } else if (log.resource == LogType::Resource::MEMORY) {
            ramMaxResolution = std::max(ramMaxResolution, log.resolution);
        } else if (log.resource == LogType::Resource::DISK) {
            hddMaxResolution = std::max(hddMaxResolution, log.resolution);
        }
    }
    cpuSamples = std::make_unique<CpuSamples>(cpuMaxResolution);
    ramSamples = std::make_unique<RamSamples>(ramMaxResolution);
    hddSamples = std::make_unique<HddSamples>(hddMaxResolution);
}

void SamplingManager::initializeLoggingBuffers(const std::vector<LogType> &logTypes) {
    for (auto &&logType : logTypes) {
        long size = 1;
        if (logType.resolution > 0) {
            size = logType.duration / logType.resolution;
        }

        if (logType.resource == LogType::Resource::CPU) {
            cpuLog[logType] = std::make_pair<CpuLog, long>(
                    std::make_shared<boost::circular_buffer<CpuState>>(size),
                    0);
        }

        if (logType.resource == LogType::Resource::MEMORY) {
            ramLog[logType] = std::make_pair<RamLog, long>(
                    std::make_shared<boost::circular_buffer<RamState>>(size),
                    0);
        }

        if (logType.resource == LogType::Resource::DISK) {
            hddLog[logType] = std::make_pair<HddLog, long>(
                    std::make_shared<boost::circular_buffer<HddState>>(size),
                    0);
        }
    }
}

void SamplingManager::initializeTriggers(const std::vector<TriggerType> &triggerTypes) {
    for (auto &&trigger : triggerTypes) {
        // initalize every trigger counter
        triggerStates[trigger] = 0;
    }
}

void SamplingManager::pollingFunction() {
    long sampleNumber = 0;
    // function working in the separate thread
    while (isSampling) {
        auto start = std::chrono::steady_clock::now();
        cpuSamples->push_back(std::make_pair<ChronoTime, CpuState>(
                std::chrono::system_clock::now(),
                resUsageProvider->getCpuState()
        ));

        ramSamples->push_back(std::make_pair<ChronoTime, RamState>(
                std::chrono::system_clock::now(),
                resUsageProvider->getRamState()
        ));

        hddSamples->push_back(std::make_pair<ChronoTime, HddState>(
                std::chrono::system_clock::now(),
                resUsageProvider->getHddState()
        ));

        processTriggers();
        processLogs();
        printDebugInfo();

        afterPollCallback(++sampleNumber);

        if (!isSampling)
            break;
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        BOOST_LOG_TRIVIAL(debug) << "Poll time: " << duration.count() << " ms";
		auto sleepTime = 0u;
		if (SAMPLING_TIME_INTERVAL_MS > duration.count())
			sleepTime = static_cast<size_t>(SAMPLING_TIME_INTERVAL_MS - duration.count());
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
    isSampling = false;
}

void SamplingManager::processTriggers() {
    // get last read values
    CpuState lastCpuState = cpuSamples->back().second;
    RamState lastRamState = ramSamples->back().second;
    HddState lastHddState = hddSamples->back().second;

    for (auto &&triggerState : triggerStates) {
        auto &&trigger = triggerState.first;
        // process different types of resources
        if (trigger.resource == TriggerType::Resource::CPU) {
            if (lastCpuState > trigger)
                triggerState.second++;
            else {
                if (triggerState.second >= triggerState.first.duration)
                    triggerDeactivate(trigger);
                triggerState.second = 0;
            }
        } else if (trigger.resource == TriggerType::Resource::MEMORY) {
            if (lastRamState > trigger)
                triggerState.second++;
            else {
                if (triggerState.second >= triggerState.first.duration)
                    triggerDeactivate(trigger);
                triggerState.second = 0;
            }
        } else if (trigger.resource == TriggerType::Resource::DISK) {
            if (lastHddState > trigger)
                triggerState.second++;
            else {
                if (triggerState.second >= triggerState.first.duration)
                    triggerDeactivate(trigger);
                triggerState.second = 0;
            }
        }

        // if trigger duration exceedes, fire it
        if (triggerState.second == triggerState.first.duration) {
            fireTrigger(trigger);
        }
    }
}

void SamplingManager::fireTrigger(const TriggerType &trigger) {
    BOOST_LOG_TRIVIAL(debug) << "!!! Trigger callback !!!";
    triggerActivate(trigger);
}

void SamplingManager::processLogs() {
    for (auto &&cpu : cpuLog) {
        auto &&logType = cpu.first;
        // get how many seconds passed since last log store
        auto &&secondsSinceLastLog = cpu.second.second;
        if (++secondsSinceLastLog >= logType.resolution) {
            // time to compute mean of (logType.duration) samples
            auto &&cpuLogBuffer = cpu.second.first;
            auto &&cpuStateMean = getCpuSamplesMean(logType.duration);
            cpuLogBuffer->push_back(std::move(cpuStateMean));
            secondsSinceLastLog = 0;
        }
    }

    for (auto &&ram : ramLog) {
        auto &&logType = ram.first;
        // get how many seconds passed since last log store
        auto &&secondsSinceLastLog = ram.second.second;
        if (++secondsSinceLastLog >= logType.resolution) {
            // time to compute mean of (logType.duration) samples
            auto &&ramLogBuffer = ram.second.first;
            auto &&ramStateMean = getRamSamplesMean(logType.duration);
            ramLogBuffer->push_back(std::move(ramStateMean));
            secondsSinceLastLog = 0;
        }
    }

    for (auto &&hdd : hddLog) {
        auto &&logType = hdd.first;
        // get how many seconds passed since last log store
        auto &&secondsSinceLastLog = hdd.second.second;
        if (++secondsSinceLastLog >= logType.resolution) {
            // time to compute mean of (logType.duration) samples
            auto &&hddLogBuffer = hdd.second.first;
            auto &&hddStateMean = getHddSamplesMean(logType.duration);
            hddLogBuffer->push_back(std::move(hddStateMean));
            secondsSinceLastLog = 0;
        }
    }
}

CpuState SamplingManager::getCpuSamplesMean(long samplesNumber) {
    long i = 0;
    // get last added state
    CpuState state = cpuSamples->rbegin()->second;
    ++i;
    // each iterator is a std::pair<std::chrono::system_time, CpuState>
    for (auto it = ++cpuSamples->rbegin(); it != cpuSamples->rend() && i < samplesNumber; ++i, ++it) {
        state += it->second;
        state /= samplesNumber;
    }
    return state;
}

RamState SamplingManager::getRamSamplesMean(long samplesNumber) {
    long i = 0;
    // get last added state
    RamState state = ramSamples->rbegin()->second;
    ++i;
    // each iterator is a std::pair<std::chrono::system_time, RamState>
    for (auto it = ++ramSamples->rbegin(); it != ramSamples->rend() && i < samplesNumber; ++i, ++it) {
        state += it->second;
        state /= samplesNumber;
    }
    return state;
}

HddState SamplingManager::getHddSamplesMean(long samplesNumber) {
    long i = 0;
    // get last added state
    HddState state = hddSamples->rbegin()->second;
    ++i;
    // each iterator is a std::pair<std::chrono::system_time, CpuState>
    for (auto it = ++hddSamples->rbegin(); it != hddSamples->rend() && i < samplesNumber; ++i, ++it) {
        state += it->second;
        state /= samplesNumber;
    }
    return state;
}

void SamplingManager::printDebugInfo() {
    BOOST_LOG_TRIVIAL(debug) << "Samples size: " << cpuSamples->size();
    for (auto &&cpu : cpuLog)
        BOOST_LOG_TRIVIAL(debug) << "CPU res: " << cpu.first.resolution << " logs size: " << cpu.second.first->size();
    for (auto &&ram : ramLog)
        BOOST_LOG_TRIVIAL(debug) << "ram res: " << ram.first.resolution << " logs size: " << ram.second.first->size();
    for (auto &&hdd : hddLog)
        BOOST_LOG_TRIVIAL(debug) << "hDD res: " << hdd.first.resolution << " logs size: " << hdd.second.first->size();
}

const std::shared_ptr<const boost::circular_buffer<CpuState>>
SamplingManager::getCpuLog(const LogType &logType) const {
    return cpuLog.at(logType).first;
}

const std::shared_ptr<const boost::circular_buffer<RamState>>
SamplingManager::getRamLog(const LogType &logType) const {
    return ramLog.at(logType).first;
}

const std::shared_ptr<const boost::circular_buffer<HddState>>
SamplingManager::getHddLog(const LogType &logType) const {
    return hddLog.at(logType).first;
}



