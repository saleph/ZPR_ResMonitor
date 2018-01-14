#include "configrawdata.hpp"


std::ostream &operator<<(std::ostream &stream, const TriggerType::FluctuationType &fluctuationType) {
    if (fluctuationType == TriggerType::FluctuationType::UNDER)
        stream << "under";
    if (fluctuationType == TriggerType::FluctuationType::OVER)
        stream << "over";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const TriggerType::Resource &resource) {
    if (resource == TriggerType::Resource::CPU)
        stream << "CPU";
    if (resource == TriggerType::Resource::MEMORY)
        stream << "RAM";
    if (resource == TriggerType::Resource::DISK)
        stream << "HDD";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const TriggerType &triggerType) {
    std::string perSecond = triggerType.resource == TriggerType::Resource::DISK ? "/s" : "";
    stream << "Trigger " << triggerType.resource << " "
           << triggerType.fluctuationType << " "
           << triggerType.triggerValue << perSecond << " "
           << "last for " << triggerType.duration << "s";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const ResourceValue::ResourceUnit &unit) {
    if (unit == ResourceValue::ResourceUnit::PERCENT)
        stream << "%";
    if (unit == ResourceValue::ResourceUnit::MB)
        stream << "MB";
    if (unit == ResourceValue::ResourceUnit::GB)
        stream << "GB";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const ResourceValue &resource) {
    stream << resource.value << resource.unitType;
    return stream;
}