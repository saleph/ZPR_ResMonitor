#include "Predicate.hpp"


Predicate::Predicate(std::function<void()> &callback)
        : callback(callback)
{}

void Predicate::activate(const TriggerType &triggerType) {
    triggerState[triggerType] = true;
    if (isPredicateSatisfied()) {
        callback();
        std::cout<<std::endl<<"Calback called!"<<std::endl<<std::endl;
    }
    else
        std::cout<<std::endl<<"Predicate not satisfied!"<<std::endl<<std::endl;
}

void Predicate::deactivate(const TriggerType &triggerType) {
    triggerState[triggerType] = false;
}
