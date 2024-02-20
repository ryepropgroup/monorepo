#include "include/labjack.hpp"
#include "include/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "LabJackM.h"
#include "../vendor/labjack/LJM_Utilities.h"

mach::LabJack::LabJack(std::string labjackName) {
    int err = INITIAL_ERR_ADDRESS;
    // LJM_Open(LJM_dtT7, LJM_ctUSB, labjackName.c_str(), &handle);
    err = LJM_Open(LJM_dtT7, LJM_ctUSB, LJM_DEMO_MODE, &handle);
    ErrorCheck(err, "LJM_Open");
}

mach::LabJack::~LabJack() {
    LJM_Close(handle);
}

int mach::LabJack::getHandle() {
    return handle;
}

void mach::LabJack::addDevice(std::shared_ptr<LJDevice<double>> floatDevice) {
    floatDevices.emplace_back(std::move(floatDevice));
    floatDevice->initialize(*this);
}

void mach::LabJack::addDevice(std::shared_ptr<LJDevice<bool>> boolDevice) {
    boolDevices.emplace_back(std::move(boolDevice));
    boolDevice->initialize(*this);
}

void mach::LabJack::setHigh(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 1);
}

void mach::LabJack::setLow(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 0);
}

template <typename T>
mach::LJDevice<T>::LJDevice(std::string name, std::string port) {
    this->name = name;
    this->port = port;
}

template <typename T>
void mach::LJDevice<T>::initialize(mach::LabJack labjack) {
    this->labjack = &labjack;
}

template <typename T>
T mach::LJDevice<T>::getValue() {
    return value;
}

template class mach::LJDevice<double>;
template class mach::LJDevice<bool>;