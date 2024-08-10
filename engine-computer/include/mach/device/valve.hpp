#pragma once
#include <string>
#include "../labjack.hpp"
#include "mach/device/device.hpp"

namespace mach {

class Valve : public Device<bool> {
    public:
        Valve(DeviceType type, std::string name, std::string port) : Device(type, name, port) {
            updateValue(false);
        }

        bool isOpen();
        void open();
        void close();
        bool isNormallyOpen() { return this->name.ends_with("NO"); }

        void print() override;
};

} // namespace mach