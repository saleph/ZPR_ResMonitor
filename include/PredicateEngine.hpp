#ifndef ZPR_MONITOR_PREDICATEENGINE_HPP
#define ZPR_MONITOR_PREDICATEENGINE_HPP


#include <mutex>  // For std::unique_lock
#include <shared_mutex>
#include <unordered_map>
#include "configrawdata.hpp"
#include "Predicate.hpp"
#include "SamplingManager.hpp"
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>


/**
 * @brief Manages events from SamplingManager if some trigger is being fired.
 *
 * Contain weak_ptr<Predicate> collection for each of TriggerType used in any Predicate.
 * If TriggerType is fired, every Predicate which contains fired trigger will receive a "signal" (simple function call)
 * of Predicate::activate(TriggerType) or Predicate::deactivate(TriggerType). If after activate() every operand of
 * particular Predicate is satisfied, the Predicate fire its own callback function.
 */
class PredicateEngine {
    friend class SamplingManager;
public:
    /// Add predicate to the engine. Engine DOES NOT obtain ownership of the predicate (stores only weak_ptr)!
    void addPredicate(std::shared_ptr<Predicate> predicate) {
        for(auto &&triggerType : predicate->getPredicateOperands()) {
            predicates[triggerType].push_back(std::move(predicate));
        }
    }

    /// Add predicates to the engine. Engine DOES NOT obtain ownership of the predicate (stores only weak_ptr)!
    void addPredicate(std::vector<std::shared_ptr<Predicate>> newPredicates) {
        for (auto &&predicate : newPredicates) {
            for (auto &&triggerType : predicate->getPredicateOperands()) {
                predicates[triggerType].push_back(predicate);
            }
        }
    }

private:
    std::function<void(const TriggerType &)> triggerActivation = [this](const TriggerType & triggerType) {
        for (auto &&predicate : predicates[triggerType]) {
            if (auto predicateObject = predicate.lock()) {
                // if predicate exists
                predicateObject->activate(triggerType);
            }
        }
        BOOST_LOG_TRIVIAL(debug) << "###### Activated: " << triggerType;
    };

    std::function<void(const TriggerType &)> triggerDeactivation = [this](const TriggerType & triggerType) {
        for (auto &&predicate : predicates[triggerType]) {
            if (auto predicateObject = predicate.lock()) {
                // if predicate exists
                predicateObject->deactivate(triggerType);
            }
        }
        BOOST_LOG_TRIVIAL(debug) << "###### Dectivated: " << triggerType;
    };

    std::unordered_map<TriggerType, std::vector<std::weak_ptr<Predicate>>> predicates;
};


#endif //ZPR_MONITOR_PREDICATEENGINE_HPP
