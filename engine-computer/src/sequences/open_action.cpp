#include <string>
#include "include/sequences/open_action.hpp"


namespace mach {

bool OpenAction::init(YAML::Node node) {
    if (!node["valve"]) {
        return false;
    }

    std::string valve = node["valve"].as<std::string>();
    

    return true;
}

void OpenAction::open() {
    valve.open();
}

} // namespace mach