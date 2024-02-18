#pragma once
#include <string>
#include <vector>
#include <memory>

namespace mach {

class LJDevice;

class LabJack {
    public:
        std::vector<std::shared_ptr<LJDevice>> devices;

        LabJack(std::string labjackName);
        ~LabJack();

        int getHandle();
        void addDevice(std::shared_ptr<LJDevice> device);
        void setHigh(std::string port);
        void setLow(std::string port);

    private:
        int handle;
};

class LJDevice {
    public:
        LJDevice(std::string name, std::string port);
        ~LJDevice();

        virtual void initialize(LabJack labjack);
        virtual void print() = 0;

    protected:
        LabJack* labjack;
        std::string name;
        std::string port;
};
}