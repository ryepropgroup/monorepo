#include "../include/device/device.hpp"

template <typename T>
mach::Device<T>::Device(std::string name, std::string port) {
    this->name = name;
    this->port = port;
}

template <typename T>
void mach::Device<T>::initialize(mach::LabJack& labjack) {
    this->labjack = &labjack;
}

template <typename T>
T mach::Device<T>::getValue() {
    return value;
}

template class mach::Device<double>;
template class mach::Device<bool>;