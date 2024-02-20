#pragma once
#include <string>
#include <vector>
#include <memory>

namespace mach {

template <typename T>
class LJDevice;

class LabJack {
    public:
        std::vector<std::shared_ptr<LJDevice<double>>> floatDevices;
        std::vector<std::shared_ptr<LJDevice<bool>>> boolDevices;

        LabJack(std::string labjackName);
        ~LabJack();

        int getHandle();
        void addDevice(std::shared_ptr<LJDevice<double>> floatDevice);
        void addDevice(std::shared_ptr<LJDevice<bool>> boolDevice);
        void setHigh(std::string port);
        void setLow(std::string port);

    private:
        int handle;
};

template <typename T>
class LJDevice {
    public:
        LJDevice(std::string name, std::string port);
        

        virtual void initialize(LabJack labjack);
        virtual T getValue();
        virtual void print() = 0;

    protected:
        LabJack* labjack;
        std::string name;
        std::string port;
        T value;
};
}