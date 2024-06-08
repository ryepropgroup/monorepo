#pragma once
#include <string>
#include <vector>
#include "../labjack.hpp"

namespace mach {

class Sensor : public Device<double> {
    public:
        std::vector<std::string> settingNames;
        std::vector<double> settingValues;

        Sensor(DeviceType type, std::string name, std::string pin, std::vector<std::string> settingNames, 
        std::vector<double> settingValues) : Device(type, name, pin), settingNames(settingNames), settingValues(settingValues) {
            this->value = 0.0;
        }

        void setLabjack(LabJack& labjack) override;
        void print() override;
};

} // namespace mach