#pragma once
#include <memory>
#include "action.hpp"
#include "../device/valve.hpp"

namespace mach {
    
class OpenAction : public Action {
    public:
        OpenAction() : Action() {}

        bool init(YAML::Node node);
        void execute() override;

    private:
        std::shared_ptr<Valve> valve;
};

}