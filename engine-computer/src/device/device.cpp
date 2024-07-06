#include "mach/device/device.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

template <typename T>
mach::Device<T>::Device(DeviceType type, std::string name, std::string port)
     : type(type), name(name), port(port), labJackPin(-1) {
    if (type == LABJACK) {
        // Convert AIN# to an integer.
        if (port.length() <= std::string{"AIN"}.length() || (!port.starts_with("AIN") && !port.starts_with("DIO"))) {
            spdlog::error("Invalid sensor port value: {}!", port);
            throw std::invalid_argument("Invalid sensor port value!");
        }
        this->labJackPin = std::stoi(port.substr(std::string{"AIN"}.length()));
    }
}

template <typename T>
void mach::Device<T>::setLabjack(std::shared_ptr<LabJack> labjack) {
    this->labjack = labjack;
}

template <typename T>
T mach::Device<T>::getValue() {
    return value;
}

template <typename T>
std::string mach::Device<T>::getName() { 
    return name; 
}

template <typename T>
void mach::Device<T>::updateValue(T value) {
    this->value = value;
}

template class mach::Device<double>;
template class mach::Device<bool>;