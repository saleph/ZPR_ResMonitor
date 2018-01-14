#ifndef INCLUDE_CONFIGRAWDATA_HPP_
#define INCLUDE_CONFIGRAWDATA_HPP_

#include <boost/functional/hash.hpp>
#include <functional>


/// Structure describing resource value of particular trigger.
/// Eg. memory over {70%, 200MB}
struct ResourceValue {
	enum class ResourceUnit		{PERCENT, MB, GB};

	/// Resource unit - percent, MB or GB
	ResourceUnit unitType;
	/// Value of resource - may be percent/size/speed
	/// distinction between size/speed is being made by context
	/// Disk has speed, memory has size
	/// Speed is being implicit interpreted as UNIT_TYPE per seconds
	long value;

    /// Operator equals used in hash maps
    bool operator==(const ResourceValue &other) const {
        return unitType == other.unitType && value == other.value;
    }
};

/// Raw structure describing trigger.
/// Used during parsing config file and
/// loading configuration into monitor.
struct TriggerType {
	enum class Resource 		{CPU, MEMORY, DISK};
	enum class FluctuationType 	{UNDER, OVER};

	/// Resource type
	Resource resource;
	/// Fluctuation type (under or over some value)
	FluctuationType fluctuationType;
	/// Fluctuation treshold
	ResourceValue triggerValue;
	/// Log duration in seconds
	long duration;

    /// Operator equals used in hash maps
    bool operator==(const TriggerType &other) const {
        return resource == other.resource && fluctuationType == other.fluctuationType
               && triggerValue == other.triggerValue && duration == other.duration;
    }
};

/// Raw structure describing logging type.
/// Used during parsing config file and
/// loading configuration into monitor.
struct LogType {
	enum class Resource	{CPU, MEMORY, DISK};

	/// Resource type
	Resource resource;
	/// Log duration in seconds
	long duration;
	/// Log resolution in seconds
	long resolution;

	/// Operator equals used in hash maps
	bool operator==(const LogType &other) const {
		return resource == other.resource && resolution == other.resolution;
	}
};

std::ostream& operator<< (std::ostream& stream, const TriggerType::FluctuationType& fluctuationType);
std::ostream& operator<< (std::ostream& stream, const TriggerType::Resource& resource);
std::ostream& operator<< (std::ostream& stream, const TriggerType& triggerType);
std::ostream& operator<< (std::ostream& stream, const ResourceValue::ResourceUnit& unit);
std::ostream& operator<< (std::ostream& stream, const ResourceValue& resource);


namespace std {
    template <>
    struct hash<LogType>
    {
        std::size_t operator()(const LogType& logType) const {
            auto resourceId = static_cast<typename std::underlying_type<LogType::Resource>::type>(logType.resource);
            long resolutionValue = logType.resolution;
            std::size_t hashValue = 0;
            boost::hash_combine(hashValue, resourceId);
            boost::hash_combine(hashValue, resolutionValue);
            return hashValue;
        }
    };
    template <>
    struct hash<TriggerType>
    {
        std::size_t operator()(const TriggerType& triggerType) const {
            auto resourceId = static_cast<typename std::underlying_type<TriggerType::Resource>::type>(
                    triggerType.resource);
            auto fluctuationType = static_cast<typename std::underlying_type<TriggerType::FluctuationType>::type>(
                    triggerType.fluctuationType);;
            auto resourceUnit = static_cast<typename std::underlying_type<ResourceValue::ResourceUnit>::type>(
                    triggerType.triggerValue.unitType);
            auto resourceValue = triggerType.triggerValue.value;
            auto duration = triggerType.duration;
            std::size_t hashValue = 0;
            boost::hash_combine(hashValue, resourceId);
            boost::hash_combine(hashValue, fluctuationType);
            boost::hash_combine(hashValue, resourceUnit);
            boost::hash_combine(hashValue, resourceValue);
            boost::hash_combine(hashValue, duration);
            return hashValue;
        }
    };
}

#endif /* INCLUDE_CONFIGRAWDATA_HPP_ */
