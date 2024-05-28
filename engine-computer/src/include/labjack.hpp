#pragma once
#include <string>
#include <vector>
#include <memory>
#include "device/device.hpp"

namespace mach {

class LabJack {
    public:
        std::vector<std::shared_ptr<Device<double>>> floatDevices;
        std::vector<std::shared_ptr<Device<bool>>> boolDevices;

        LabJack(std::string labjackName);
        ~LabJack();
        LabJack(const LabJack&) = delete; // No copying! >:(
        LabJack& operator=(const LabJack&) = delete;

        int getHandle();
        void addDevice(std::shared_ptr<Device<double>> floatDevice);
        void addDevice(std::shared_ptr<Device<bool>> boolDevice);
        void setHigh(std::string port);
        void setLow(std::string port);

    private:
        int handle;
};

} // namespace mach