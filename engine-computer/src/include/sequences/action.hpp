#pragma once

#include <yaml-cpp/yaml.h>

namespace mach {

    class Action {
        public:
            std::string type;

            Action(std::string type) : type(type) {}

            virtual bool init(YAML::Node node) = 0;
    };

    void initActions();

    Action& fromConfig(YAML::Node node);
}