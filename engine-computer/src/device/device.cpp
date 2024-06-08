#include "include/device/device.hpp"

template <typename T>
void mach::Device<T>::setLabjack(mach::LabJack& labjack) {
    this->labjack = &labjack;
}

template <typename T>
T mach::Device<T>::getValue() {
    return value;
}

template class mach::Device<double>;
template class mach::Device<bool>;