#pragma once
#include <string>
#include <memory>
#include <atomic>

namespace mach {

enum DeviceType {
    LABJACK,
    REMOTE
};

class LabJack;

template <typename T>
class Device {
    public:
        Device(DeviceType type, std::string name, std::string port);
        virtual ~Device() = default;
        
        virtual void setLabjack(std::shared_ptr<LabJack> labjack);
        virtual T getValue();
        virtual void print() = 0;
        std::string getName();
        void updateValue(T value);
        int getLabJackPin();
        std::string getPort();

    protected:
        DeviceType type;
        std::shared_ptr<LabJack> labjack;
        std::string name;
        std::string port;
        int labJackPin;
        std::atomic<T> value;
};

} // namespace mach