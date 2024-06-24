#include <string>
#include "mach/sequences/open_action.hpp"
#include "mach/device_manager.hpp"


namespace mach {

bool OpenAction::init(YAML::Node node) {
    if (!node["valve"]) {
        return false;
    }
    std::string valveName = node["valve"].as<std::string>();
    std::shared_ptr<Valve> valve = DeviceManager::getInstance().getValve(valveName);
    if (!valve) {
        return false;
    }
    this->valve = valve;
    return true;
}

void OpenAction::execute() {
    valve->open();
}

} // namespace mach