#pragma once

#include <yaml-cpp/yaml.h>

namespace mach {

    /**
     * @brief An action in a control sequence. See mach::ActionFactory in order to instantiate actions.
     */
    class Action {
        public:
            Action() = default;
            virtual ~Action() = default;

            /**
             * @brief Initializes the action by parsing the action's info from the config.
             * 
             * @param node The YAML node for this action, from the config.
             * @return true if the action was parsed successfully, false otherwise.
             */
            virtual bool init(YAML::Node node) = 0;

            /**
             * @brief Executes this action.
             */
            virtual void execute() = 0;
    };
}