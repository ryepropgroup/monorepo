#pragma once
#include <string>
#include <vector>
#include "labjack.hpp"

namespace mach {

class Sensor : public LJDevice<double> {
    public:
        std::vector<std::string> settingNames;
        std::vector<double> settingValues;

        Sensor(std::string name, std::string pin, std::vector<std::string> settingNames, 
               std::vector<double> settingValues);

        void initialize(LabJack labjack) override;
        void print() override;
};

}