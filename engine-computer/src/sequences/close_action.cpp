#include <string>
#include "mach/sequences/close_action.hpp"
#include "mach/device_manager.hpp"


namespace mach {

bool CloseAction::init(YAML::Node node) {
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

void CloseAction::execute() {
    valve->close();
}

} // namespace mach