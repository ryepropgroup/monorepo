#pragma once

#include "./action.hpp"
#include "../device/valve.hpp"

namespace mach {
    
class OpenAction : public Action {
    public:
        OpenAction(Valve& valve) : Action("OPEN"), valve(valve) {}

        bool init(YAML::Node node);
        void open();

    private:
        Valve& valve;
};

}