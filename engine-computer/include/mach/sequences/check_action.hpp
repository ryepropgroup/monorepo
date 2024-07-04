#pragma once
#include <memory>
#include "action.hpp"
#include "mach/device/valve.hpp"

namespace mach {
    
class CheckAction : public Action {
    public:
        CheckAction() : Action() {}

        bool init(YAML::Node node);
        void execute() override;

    private:
        std::shared_ptr<Valve> valve;
        double target;
};

}