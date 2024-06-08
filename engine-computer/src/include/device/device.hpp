#pragma once
#include <string>

namespace mach {

enum DeviceType {
    LABJACK,
    REMOTE
};

class LabJack;

template <typename T>
class Device {
    public:
        Device(DeviceType type, std::string name, std::string port) : type(type), name(name), port(port) {}

        virtual void setLabjack(LabJack& labjack);
        virtual T getValue();
        virtual void print() = 0;

    protected:
        DeviceType type;
        LabJack* labjack;
        std::string name;
        std::string port;
        T value;
};

} // namespace mach