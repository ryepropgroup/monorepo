#pragma once
#include <string>
#include <vector>
#include "mach/labjack.hpp"
#include "mach/device/device.hpp"

namespace mach {

class Sensor : public Device<double> {
    public:
        std::pair<std::vector<std::string>, std::vector<double>> labJackSettings;

        Sensor(DeviceType type, std::string name, std::string port);

        void setLabjack(std::shared_ptr<LabJack> labjack) override;

        void setThermocoupleType(std::string type);
        void setRange(double range);
        void setMultiplier(double multiplier);
        void setNegativeChannel(std::string channel);
        std::string getThermocoupleType();

        void updateValue(double value);
        
        void print() override;
    private:
        std::string thermocoupleType = "";
        double range = 0;
        double multiplier = 1.0;
        std::string negativeChannel = "";

};

} // namespace mach