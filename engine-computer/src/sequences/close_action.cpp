#include <string>
#include <spdlog/spdlog.h>
#include "mach/sequences/close_action.hpp"
#include "mach/device/device_manager.hpp"

namespace mach {

bool CloseAction::init(YAML::Node node) {
    if (!node["valve"]) {
        spdlog::error("MACH: Close action is missing 'valve' field.");
        return false;
    }
    std::string valveName = node["valve"].as<std::string>();
    std::shared_ptr<Valve> valve = DeviceManager::getInstance().getValve(valveName);
    if (!valve) {
        spdlog::error("MACH: Valve '{}' for close action doesn't exist.", valveName);
        return false;
    }
    this->valve = valve;
    return true;
}

void CloseAction::execute(bool override) {
    valve->close();
}

void CloseAction::setValve(std::shared_ptr<Valve> valve) {
    this->valve = valve;
}

} // namespace mach