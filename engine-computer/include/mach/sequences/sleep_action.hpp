#pragma once
#include <memory>
#include "action.hpp"
#include "mach/device/valve.hpp"

namespace mach {
    
class SleepAction : public Action {
    public:
        SleepAction() : Action() {}

        bool init(YAML::Node node);
        void execute(bool override) override;

    private:
        double duration;
};

}