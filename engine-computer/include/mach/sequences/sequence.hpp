#pragma once
#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include "mach/sequences/action.hpp"
#include "mach/device/device_manager.hpp"

namespace mach {
    
class Sequence {
    public:
        Sequence(std::string name) : name(name), actions() {
            this->override = name == "abort";
        }

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
            DeviceManager& deviceManager = DeviceManager::getInstance();
            for (auto& action : actions) {
                if (deviceManager.isAborting() && !this->override) {
                    spdlog::info("Aborting sequence '{}'", name);
                    return;
                }
                action->execute(this->override);
            }
        }

        std::string getName() {
            return name;
        }

        bool isOverride() {
            return override;
        }

    private:
        std::string name;
        std::list<std::unique_ptr<Action>> actions;
        bool override = false;
};

}