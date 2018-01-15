#include "PredicateTypes.hpp"


Predicate_1or2and3::Predicate_1or2and3(std::function<void()> callback, const TriggerType &or1Operand,
                                       const TriggerType &or2Operand, const TriggerType &andOperand)
        : Predicate(callback), or1Operand(or1Operand), or2Operand(or2Operand), andOperand(andOperand)
{
    for (auto &&trigger : getPredicateOperands()) {
        triggerState[trigger] = false;
    }
}

std::vector<TriggerType> Predicate_1or2and3::getPredicateOperands() const {
    return {or1Operand, or2Operand, andOperand};
}

bool Predicate_1or2and3::isPredicateSatisfied() const {

    if (triggerState.at(or1Operand) && triggerState.at(or2Operand) && lastSatisfied && triggerState.at(andOperand)) {
        // thats mean that a trigger has been already fired: second satisfaction of OR should not cause
        // another fire
        return false;
    }
    lastSatisfied = (triggerState.at(or1Operand) || triggerState.at(or2Operand)) && triggerState.at(andOperand);
    return lastSatisfied;
}

std::ostream & Predicate_1or2and3::print(std::ostream & stream) const
{
	stream << or1Operand << " OR " << or2Operand << " AND " << andOperand;
	return stream;
}

Predicate_1and2::Predicate_1and2(std::function<void()> callback, const TriggerType &and1Operand,
                                 const TriggerType &and2Operand)
        : Predicate(callback), and1Operand(and1Operand), and2Operand(and2Operand)
{
    for (auto &&trigger : getPredicateOperands()) {
        triggerState[trigger] = false;
    }
}

std::vector <TriggerType> Predicate_1and2::getPredicateOperands() const {
    return {and1Operand, and2Operand};
}

bool Predicate_1and2::isPredicateSatisfied() const {
    return triggerState.at(and1Operand) && triggerState.at(and2Operand);
}

std::ostream & Predicate_1and2::print(std::ostream & stream) const
{
	stream << and1Operand << " AND " << and2Operand;
	return stream;
}

Predicate_1element::Predicate_1element(std::function<void()> callback, const TriggerType &operand)
        : Predicate(callback), operand(operand)
{
    for (auto &&trigger : getPredicateOperands()) {
        triggerState[trigger] = false;
    }
}

std::vector <TriggerType> Predicate_1element::getPredicateOperands() const {
    return {operand};
}

bool Predicate_1element::isPredicateSatisfied() const {
    return triggerState.at(operand);
}

std::ostream & Predicate_1element::print(std::ostream & stream) const
{
	stream << operand;
	return stream;
}
