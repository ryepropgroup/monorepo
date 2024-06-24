#include <string>
#include <chrono>
#include <spdlog/spdlog.h>
#include "mach/sequences/sleep_action.hpp"
#include "mach/device_manager.hpp"
#include "mach/util.hpp"

namespace mach {

bool SleepAction::init(YAML::Node node) {
    if (!node["duration"]) {
        return false;
    }
    this->duration = node["duration"].as<double>();
    if (this->duration <= 0.0) {
        spdlog::error("Invalid sleep action defined (duration of {}s)", this->duration);
        return false;
    }
    return true;
}

void SleepAction::execute() {
    util::sleepOrAbort(std::chrono::milliseconds(static_cast<long>(duration * 1000.0)));
}

} // namespace mach