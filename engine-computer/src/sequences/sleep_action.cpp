#include <string>
#include <chrono>
#include <spdlog/spdlog.h>
#include "include/sequences/sleep_action.hpp"
#include "include/device_manager.hpp"
#include "include/util.hpp"

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