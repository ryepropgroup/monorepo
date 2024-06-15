#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "sequences/action.hpp"

namespace mach {

class ActionFactory {
    public:
        template <typename T>
        void registerAction(const std::string& action) {
            actionCreators[action] = []() { return std::make_unique<T>(); };
        }

        /**
         * @brief Creates an action from the sequence config.
         * 
         * @param node The YAML node from the sequence config.
         * @return std::unique_ptr<Action> The created action.
         */
        std::unique_ptr<Action> createAction(const YAML::Node& node) {
            std::string actionName = node["action"].as<std::string>();
            std::unique_ptr<Action> action = createAction(actionName);
            if (!action) {
                return nullptr;
            }
            if (action->init(node)) {
                return action;
            }
            return nullptr;
        }

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<Action>()>> actionCreators;

        std::unique_ptr<Action> createAction(const std::string& action) {
            auto it = actionCreators.find(action);
            if (it != actionCreators.end()) {
                return it->second();
            }
            return nullptr;
        }
};

}