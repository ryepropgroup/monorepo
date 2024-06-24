#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <yaml-cpp/yaml.h>
#include "action.hpp"

namespace mach {

/**
 * @brief Factory class for creating actions.
 */
class ActionFactory {
    public:
        static ActionFactory& getInstance() {
            static ActionFactory instance;
            return instance;
        }

        ActionFactory(ActionFactory const&) = delete;
        void operator=(ActionFactory const&) = delete;

        void registerAction(const std::string& action, std::function<std::unique_ptr<Action>()> creator);

        /**
        * @brief Creates an action from the sequence config.
        * 
        * @param node The YAML node from the sequence config.
        * @return std::unique_ptr<Action> The created action.
        */
        std::unique_ptr<Action> createAction(const YAML::Node& node);
    
    private:
        ActionFactory() {}
    };
}