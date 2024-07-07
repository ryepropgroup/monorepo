#pragma once
#include <memory>
#include "action.hpp"
#include "mach/device/valve.hpp"

namespace mach {
    
class CloseAction : public Action {
    public:
        CloseAction() : Action() {}

        bool init(YAML::Node node);
        void setValve(std::shared_ptr<Valve> valve);
        void execute() override;

    private:
        std::shared_ptr<Valve> valve;
};

}