#pragma once
#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include "mach/sequences/action.hpp"

namespace mach {
    
class Sequence {
    public:
        Sequence(std::string name) : name(name), actions() {}

        /**
         * @brief Adds an action to the sequence.
         * 
         * @param action A unique pointer to the action.
         */
        void addAction(std::unique_ptr<Action> action) {
            actions.push_back(std::move(action));
        }

        /**
         * @brief Executes a sequence.
         * 
         * @param sequenceName The name of the sequence to execute.
         */
        void execute() {
            for (auto& action : actions) {
                action->execute();
            }
        }

    private:
        std::string name;
        std::list<std::unique_ptr<Action>> actions;
};

}