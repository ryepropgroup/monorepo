#pragma once
#include <string>

namespace mach {

class LabJack;

template <typename T>
class Device {
    public:
        Device(std::string name, std::string port);
        
        virtual void initialize(LabJack& labjack);
        virtual T getValue();
        virtual void print() = 0;

    protected:
        LabJack* labjack;
        std::string name;
        std::string port;
        T value;
};

} // namespace mach