#pragma once

#include <memory>
#include "./action.hpp"
#include "../device/valve.hpp"

namespace mach {
    
class SleepAction : public Action {
    public:
        SleepAction() : Action() {}

        bool init(YAML::Node node);
        void execute() override;

    private:
        double duration;
};

}