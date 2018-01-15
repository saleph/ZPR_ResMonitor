#ifndef ZPR_PREDICATE_HPP
#define ZPR_PREDICATE_HPP


#include <functional>
#include <unordered_map>
#include "configrawdata.hpp"
#include <iostream>

/**
 * @brief Abstract interface for Predicate computations.
 * If the derivative class' isPredicateSatisfied() become true, it fire the callback function.
 */
class Predicate {
public:
    /// Constructor of the Predicate body.
    /// Takes function to call after the predicate met.
    explicit Predicate(std::function<void()> &callback);
    
    virtual ~Predicate() = default;

    /// Returns vector of TriggerType used in particular Predicate.
    /// Used in PredicateEngine.
    virtual std::vector<TriggerType> getPredicateOperands() const = 0;

    /// Checks if the Predicate is met.
    virtual bool isPredicateSatisfied() const = 0;

    /// If trigger is fired, set its value to true and perform check if the whole Predicate is now true.
    void activate(const TriggerType &triggerType);

    /// Sets trigger's value to false.
    void deactivate(const TriggerType &triggerType);

protected:
    std::unordered_map<TriggerType, bool> triggerState;
    std::function<void()> callback;
};

#endif //ZPR_PREDICATE_HPP
