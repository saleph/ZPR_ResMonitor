#ifndef ZPR_MONITOR_SAMPLESMANAGER_HPP
#define ZPR_MONITOR_SAMPLESMANAGER_HPP

#include <boost/circular_buffer.hpp>
#include <unordered_map>
#include <utility>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <memory>
#include <iostream>
#include "configrawdata.hpp"
#include "CpuState.hpp"
#include "RamState.hpp"
#include "HddState.hpp"
#include <thread>
#include <chrono>
#include "ResUsageProvider.hpp"


/**
 * 	@brief	Sampler management class.
 * 	Stores information about state samples.
 */
class SamplingManager {
    const size_t SAMPLES_BUFFER_SIZE = 3600;
    const size_t SAMPLING_TIME_MS = 1000;
public:
    SamplingManager(ResUsageProvider && resUsageProvider,
            const std::vector<LogType> &logTypes,
                    const std::vector<TriggerType> &triggerType);

    ~SamplingManager();

    const std::shared_ptr<const boost::circular_buffer<CpuState>>&
    getCpuLog(const LogType &logType) const;

    const std::shared_ptr<const boost::circular_buffer<RamState>>&
    getRamLog(const LogType &logType) const;

    const std::shared_ptr<const boost::circular_buffer<HddState>>&
    getHddLog(const LogType &logType) const;

private:
    using ChronoTime = std::chrono::time_point<std::chrono::system_clock>;
    template <class T>
    using SamplesBuffer = boost::circular_buffer<std::pair<ChronoTime, T>>;
    using CpuSamples = SamplesBuffer<CpuState>;
    using RamSamples = SamplesBuffer<RamState>;
    using HddSamples = SamplesBuffer<HddState>;
    using CpuLog = std::shared_ptr<boost::circular_buffer<CpuState>>;
    using RamLog = std::shared_ptr<boost::circular_buffer<RamState>>;
    using HddLog = std::shared_ptr<boost::circular_buffer<HddState>>;

    ResUsageProvider resUsageProvider;
    std::thread pollerThread;
    std::atomic_bool isSampling;

    // most discrete samples - used for triggers and logs
    CpuSamples cpuSamples;
    RamSamples ramSamples;
    HddSamples hddSamples;

    // logging circular buffers
    std::unordered_map<LogType, CpuLog> cpuLog;
    std::unordered_map<LogType, RamLog> ramLog;
    std::unordered_map<LogType, HddLog> hddLog;

    std::unordered_map<TriggerType, long> triggerStates;

    void initializeLoggingBuffers(const std::vector<LogType> &logTypes);

    void pollingFunction();

    void processTriggers();

    void processLogs();

    void fireTrigger(const TriggerType &trigger);
};


#endif //ZPR_MONITOR_SAMPLESMANAGER_HPP
