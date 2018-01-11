#include "SamplingManager.hpp"



void SamplingManager::initializeLoggingBuffers(const std::vector<LogType> &logTypes) {
    for (auto &&logType : logTypes) {
        long size = 1;
        if (logType.resolution > 0) {
            size = logType.duration / logType.resolution;
        }
        if (logType.resource == LogType::Resource::CPU) {
            cpuLog[logType] = std::make_shared<boost::circular_buffer<CpuState>>(size);
        }

        if (logType.resource == LogType::Resource::MEMORY) {
            ramLog[logType] = std::make_shared<boost::circular_buffer<RamState>>(size);
        }

        if (logType.resource == LogType::Resource::DISK) {
            hddLog[logType] = std::make_shared<boost::circular_buffer<HddState>>(size);
        }
    }
}

void SamplingManager::pollingFunction() {
    while (isSampling) {
        BOOST_LOG_TRIVIAL(debug) << "hello";
        auto start = std::chrono::steady_clock::now();
        cpuSamples.push_back(std::make_pair<ChronoTime, CpuState>(
                std::chrono::system_clock::now(),
                resUsageProvider.getCpuState()
        ));

        ramSamples.push_back(std::make_pair<ChronoTime, RamState>(
                std::chrono::system_clock::now(),
                resUsageProvider.getRamState()
        ));

        hddSamples.push_back(std::make_pair<ChronoTime, HddState>(
                std::chrono::system_clock::now(),
                resUsageProvider.getHddState()
        ));



        if (!isSampling)
            break;
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        BOOST_LOG_TRIVIAL(debug) << "Poll time: " << duration.count() << " ms";
        size_t sleepTime = static_cast<size_t>(SAMPLING_TIME_MS - duration.count());
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}

SamplingManager::~SamplingManager() {
    isSampling = false;
    pollerThread.join();
    BOOST_LOG_TRIVIAL(debug) << "joined";
}

SamplingManager::SamplingManager(ResUsageProvider &&resUsageProvider,
                                 const std::vector<LogType> &logTypes,
                                 const std::vector<TriggerType> &triggerType)
        : cpuSamples(SAMPLES_BUFFER_SIZE),
          ramSamples(SAMPLES_BUFFER_SIZE),
          hddSamples(SAMPLES_BUFFER_SIZE),
          isSampling(true),
          resUsageProvider(std::move(resUsageProvider))
{
    initializeLoggingBuffers(logTypes);
    pollerThread = std::thread(&SamplingManager::pollingFunction, this);
}

const std::shared_ptr<const boost::circular_buffer<CpuState>> &
SamplingManager::getCpuLog(const LogType &logType) const {
    return cpuLog.at(logType);
}

const std::shared_ptr<const boost::circular_buffer<RamState>> &
SamplingManager::getRamLog(const LogType &logType) const {
    return ramLog.at(logType);
}

const std::shared_ptr<const boost::circular_buffer<HddState>> &
SamplingManager::getHddLog(const LogType &logType) const {
    return hddLog.at(logType);
}

