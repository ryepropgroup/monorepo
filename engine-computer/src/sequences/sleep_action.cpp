#include <string>
#include <chrono>
#include <spdlog/spdlog.h>
#include "mach/sequences/sleep_action.hpp"
#include "mach/device/device_manager.hpp"
#include "mach/util.hpp"

namespace mach {

bool SleepAction::init(YAML::Node node) {
    if (!node["duration"]) {
        spdlog::error("MACH: Sleep action is missing 'duration' field.");
        return false;
    }
    this->duration = node["duration"].as<double>();
    if (this->duration <= 0.0) {
        spdlog::error("MACH: Sleep action has invalid duration of '{}'.", this->duration);
        return false;
    }
    return true;
}

void SleepAction::execute() {
    spdlog::info("MACH: Sleeping for {} seconds.", duration);
    util::sleepOrAbort(std::chrono::milliseconds(static_cast<long>(duration * 1000.0)));
}

} // namespace mach