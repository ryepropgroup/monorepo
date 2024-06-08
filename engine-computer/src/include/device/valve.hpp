#pragma once
#include <string>
#include "../labjack.hpp"

namespace mach {

class Valve : public Device<bool> {
    public:
        Valve(DeviceType type, std::string name, std::string port) : Device(type, name, port) {
            this->value = false;
        }

        bool isOpen();
        void open();
        void close();

        void print() override;
};

} // namespace mach