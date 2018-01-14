#include "Predicate.hpp"


Predicate::Predicate(std::function<void()> &callback)
        : callback(callback)
{}

void Predicate::activate(const TriggerType &triggerType) {
    triggerState[triggerType] = true;
    if (isPredicateSatisfied()) {
        callback();
    }
}

void Predicate::deactivate(const TriggerType &triggerType) {
    triggerState[triggerType] = false;
}