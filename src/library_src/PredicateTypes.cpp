#include "PredicateTypes.hpp"


Predicate_1or2and3::Predicate_1or2and3(std::function<void()> &callback, const TriggerType &or1Operand,
                                       const TriggerType &or2Operand, const TriggerType &andOperand)
        : Predicate(callback), or1Operand(or1Operand), or2Operand(or2Operand), andOperand(andOperand)
{}

std::vector<TriggerType> Predicate_1or2and3::getPredicateOperands() const {
    return {or1Operand, or2Operand, andOperand};
}

bool Predicate_1or2and3::isPredicateSatisfied() const {
    return (triggerState.at(or1Operand) || triggerState.at(or2Operand)) && triggerState.at(andOperand);
}

Predicate_1and2::Predicate_1and2(std::function<void()> &callback, const TriggerType &and1Operand,
                                 const TriggerType &and2Operand)
        : Predicate(callback), and1Operand(and1Operand), and2Operand(and2Operand)
{}

std::vector <TriggerType> Predicate_1and2::getPredicateOperands() const {
    return {and1Operand, and2Operand};
}

bool Predicate_1and2::isPredicateSatisfied() const {
    return triggerState.at(and1Operand) && triggerState.at(and2Operand);
}

Predicate_1element::Predicate_1element(std::function<void()> &callback, const TriggerType &operand)
        : Predicate(callback), operand(operand)
{}

std::vector <TriggerType> Predicate_1element::getPredicateOperands() const {
    return {operand};
}

bool Predicate_1element::isPredicateSatisfied() const {
    return triggerState.at(operand);
}
