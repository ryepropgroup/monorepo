#include "mach/device/device.hpp"

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

template class mach::Device<double>;
template class mach::Device<bool>;