#include "mach/sequences/action_factory.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "mach/sequences/action.hpp"

namespace mach {

    static std::unordered_map<std::string, std::function<std::unique_ptr<Action>()>> actionCreators;

    static std::unique_ptr<Action> findAction(const std::string& action) {
        auto it = actionCreators.find(action);
        if (it != actionCreators.end()) {
            return it->second();
        }
        return nullptr;
    } 

    void ActionFactory::registerAction(const std::string& action, std::function<std::unique_ptr<Action>()> creator) {
        actionCreators[action] = creator;
    }

    std::unique_ptr<Action> ActionFactory::createAction(const YAML::Node& node) {
        std::string actionName = node["action"].as<std::string>();
        std::unique_ptr<Action> action = findAction(actionName);
        if (!action) {
            return nullptr;
        }
        if (action->init(node)) {
            return action;
        }
        return nullptr;
    }

} // namespace mach