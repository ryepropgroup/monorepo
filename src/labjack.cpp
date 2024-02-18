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

void mach::LabJack::addDevice(std::shared_ptr<LJDevice> device) {
    devices.push_back(device);
    device->initialize(*this);
}

void mach::LabJack::setHigh(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 1);
}

void mach::LabJack::setLow(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 0);
}

mach::LJDevice::LJDevice(std::string name, std::string port) {
    this->name = name;
    this->port = port;
}

mach::LJDevice::~LJDevice() {
}

void mach::LJDevice::initialize(mach::LabJack labjack) {
    this->labjack = &labjack;
}