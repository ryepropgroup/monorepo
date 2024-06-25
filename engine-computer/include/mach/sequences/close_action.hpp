#pragma once
#include <memory>
#include "action.hpp"
#include "../device/valve.hpp"

namespace mach {
    
class CloseAction : public Action {
    public:
        CloseAction() : Action() {}

        bool init(YAML::Node node);
        void execute() override;

    private:
        std::shared_ptr<Valve> valve;
};

}