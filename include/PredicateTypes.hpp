#ifndef ZPR_MONITOR_PREDICATETYPES_HPP
#define ZPR_MONITOR_PREDICATETYPES_HPP

#include "Predicate.hpp"
#include <functional>
#include <configrawdata.hpp>


/**
 * @brief Predicate of type ((trigger1 OR trigger2) AND trigger3).
 */
class Predicate_1or2and3 : public Predicate {
public:
    /// Constructs this predicate. Takes callback function and operands.
    explicit Predicate_1or2and3(std::function<void()> callback,
                                const TriggerType &or1Operand, const TriggerType &or2Operand,
                                const TriggerType &andOperand);

    virtual ~Predicate_1or2and3() = default;

    /// Returns vector of TriggerType used in this Predicate.
    /// Used in PredicateEngine.
    virtual std::vector<TriggerType> getPredicateOperands() const;

    /// Checks if the Predicate is met.
    virtual bool isPredicateSatisfied() const;

    /// Print method for predicate printing
    virtual std::ostream & print(std::ostream & stream) const;
private:
    mutable bool lastSatisfied = false;
    TriggerType or1Operand;
    TriggerType or2Operand;
    TriggerType andOperand;
};

/**
 * @brief Predicate of type (trigger1 AND trigger2).
 */
class Predicate_1and2 : public Predicate {
public:
    /// Constructs this predicate. Takes callback function and operands.
    explicit Predicate_1and2(std::function<void()> callback,
                             const TriggerType &and1Operand, const TriggerType &and2Operand);

    virtual ~Predicate_1and2() = default;

    /// Returns vector of TriggerType used in this Predicate.
    /// Used in PredicateEngine.
    virtual std::vector<TriggerType> getPredicateOperands() const;

    /// Checks if the Predicate is met.
    virtual bool isPredicateSatisfied() const;

    /// Print method for predicate printing
    virtual std::ostream & print(std::ostream & stream) const;
private:
    TriggerType and1Operand;
    TriggerType and2Operand;
};

/**
 * @brief Predicate of type (trigger1).
 */
class Predicate_1element : public Predicate {
public:
    /// Constructs this predicate. Takes callback function and operands.
    explicit Predicate_1element(std::function<void()> callback,
                                const TriggerType &operand);

    virtual ~Predicate_1element() = default;

    /// Returns vector of TriggerType used in this Predicate.
    /// Used in PredicateEngine.
    virtual std::vector<TriggerType> getPredicateOperands() const;

    /// Checks if the Predicate is met.
    virtual bool isPredicateSatisfied() const;

    virtual std::ostream & print(std::ostream & stream) const;
private:
    TriggerType operand;
};

#endif //ZPR_MONITOR_PREDICATETYPES_HPP
