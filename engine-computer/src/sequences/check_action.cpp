#include "mach/sequences/check_action.hpp"
#include <spdlog/spdlog.h>
#include "mach/util.hpp"
#include "mach/device/device_manager.hpp"

namespace mach {

bool CheckAction::init(YAML::Node node) {
    if (!node["sensor"]) {
        spdlog::error("MACH: Check action is missing 'sensor' field.");
        return false;
    }
    std::string sensorName = node["sensor"].as<std::string>();
    std::shared_ptr<Sensor> sensor = DeviceManager::getInstance().getSensor(sensorName);
    if (!sensor) {
        spdlog::error("MACH: Sensor '{}' for check action doesn't exist.", sensorName);
        return false;
    }
    this->sensor = sensor;

    if (!node["threshold"]) {
        spdlog::error("MACH: Check action is missing 'threshold' field.");
        return false;
    }
    this->threshold = node["threshold"].as<double>();
    return true;
}

void CheckAction::execute() {
    spdlog::info("MACH: Waiting on sensor '{}' for value {}.", sensor->getName(), threshold);
    
    // while (sensor->getValue() < threshold) {
    //     // spdlog::info("MACH: Sensor '{}' value is '{}'.", sensor->getName(), sensor->getValue());
    //     util::sleepOrAbort(std::chrono::milliseconds(1));
    // }

    spdlog::info("MACH: Sensor '{}' value has reached treshold at '{}'.", sensor->getName(), sensor->getValue());
}

} // namespace mach