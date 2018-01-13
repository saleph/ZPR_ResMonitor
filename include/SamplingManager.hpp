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
 * 	Collects, stores and analize data from ResUsageProvider.
 * 	1. Constantly log state of the OS into desired buffers specified by 'LogType's.
 */
class SamplingManager {
public:

    SamplingManager(std::shared_ptr<ResUsageProvider> &resUsageProvider,
                    const std::vector<LogType> &logTypes,
                    const std::vector<TriggerType> &triggerTypes,
                    std::function<void(const TriggerType &)> triggerCallback = [](const TriggerType &) { });

    ~SamplingManager();

    const std::shared_ptr<const boost::circular_buffer<CpuState>>
    getCpuLog(const LogType &logType) const;

    const std::shared_ptr<const boost::circular_buffer<RamState>>
    getRamLog(const LogType &logType) const;

    const std::shared_ptr<const boost::circular_buffer<HddState>>
    getHddLog(const LogType &logType) const;

    void startSampling();

    void stopSampling();

private:
    using ChronoTime = std::chrono::time_point<std::chrono::system_clock>;
    template<class T>
    using SamplesBuffer = boost::circular_buffer<std::pair<ChronoTime, T>>;
    using CpuSamples = SamplesBuffer<CpuState>;
    using RamSamples = SamplesBuffer<RamState>;
    using HddSamples = SamplesBuffer<HddState>;
    using CpuLog = std::shared_ptr<boost::circular_buffer<CpuState>>;
    using RamLog = std::shared_ptr<boost::circular_buffer<RamState>>;
    using HddLog = std::shared_ptr<boost::circular_buffer<HddState>>;

    std::shared_ptr<ResUsageProvider> resUsageProvider;
    std::function<void(const TriggerType &)> triggerCallback;

    // most discrete samples - used for logs
    std::unique_ptr<CpuSamples> cpuSamples;
    std::unique_ptr<RamSamples> ramSamples;
    std::unique_ptr<HddSamples> hddSamples;

    // logging circular buffers
    // LogType: describes each log
    // std::pair<CpuLog buffer, time since last update>
    std::unordered_map<LogType, std::pair<CpuLog, long>> cpuLog;
    std::unordered_map<LogType, std::pair<RamLog, long>> ramLog;
    std::unordered_map<LogType, std::pair<HddLog, long>> hddLog;

    // describes exceed time of each trigger - how many read polls exceeded a trigger
    std::unordered_map<TriggerType, long> triggerStates;

protected:
    size_t SAMPLING_TIME_INTERVAL_MS = 1000;
    std::thread pollerThread;
    // mostly debug callback, takes samples got
    std::function<void(long)> afterPollCallback = [](long){};
    std::atomic_bool isSampling;

private:

    void initializeSamplesBuffers(const std::vector<LogType> &logTypes);

    void initializeLoggingBuffers(const std::vector<LogType> &logTypes);

    void initializeTriggers(const std::vector<TriggerType> &triggerTypes);

    void pollingFunction();

    void processTriggers();

    void processLogs();

    void fireTrigger(const TriggerType &trigger);

    CpuState getCpuSamplesMean(long samplesNumber);

    RamState getRamSamplesMean(long samplesNumber);

    HddState getHddSamplesMean(long samplesNumber);

    void printDebugInfo();
};


#endif //ZPR_MONITOR_SAMPLESMANAGER_HPP
