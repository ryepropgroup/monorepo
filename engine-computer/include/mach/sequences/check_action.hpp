#pragma once
#include <memory>
#include "action.hpp"
#include "mach/device/sensor.hpp"

namespace mach {
    
class CheckAction : public Action {
    public:
        CheckAction() : Action() {}

        bool init(YAML::Node node);
        void execute(bool override) override;

    private:
        std::shared_ptr<Sensor> sensor;
        double threshold;
};

}