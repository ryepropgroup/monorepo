#pragma once
#include <memory>
#include "action.hpp"
#include "mach/device/valve.hpp"

namespace mach {
    
class OpenAction : public Action {
    public:
        OpenAction() : Action() {}

        bool init(YAML::Node node);
        void setValve(std::shared_ptr<Valve> valve);
        void execute(bool override) override;

    private:
        std::shared_ptr<Valve> valve;
};

}