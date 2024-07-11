#include "mach/sequences/open_action.hpp"
#include <string>
#include <spdlog/spdlog.h>
#include "mach/device/device_manager.hpp"

namespace mach {

bool OpenAction::init(YAML::Node node) {
    if (!node["valve"]) {
        spdlog::error("MACH: Open action is missing 'valve' field.");
        return false;
    }
    std::string valveName = node["valve"].as<std::string>();
    std::shared_ptr<Valve> valve = DeviceManager::getInstance().getValve(valveName);
    if (!valve) {
        spdlog::error("MACH: Valve '{}' for open action doesn't exist.", valveName);
        return false;
    }
    this->valve = valve;
    return true;
}

void OpenAction::execute(bool override) {
    valve->open();
}

void OpenAction::setValve(std::shared_ptr<Valve> valve) {
    this->valve = valve;
}

} // namespace mach