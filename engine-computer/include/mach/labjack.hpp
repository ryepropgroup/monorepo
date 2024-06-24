#pragma once
#include <string>
#include <vector>
#include <memory>
#include "device/device.hpp"

namespace mach {

class LabJack {
    public:
        LabJack(std::string labjackName);
        ~LabJack();
        LabJack(const LabJack&) = delete; // No copying! >:(
        LabJack& operator=(const LabJack&) = delete;

        int getHandle();
        void setHigh(std::string port);
        void setLow(std::string port);

    private:
        int handle;
};

} // namespace mach