#include "include/labjack.hpp"
#include "include/sensor.hpp"
#include "include/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "LabJackM.h"
#include "../vendor/labjack/LJM_Utilities.h"
#include <boost/exception/diagnostic_information.hpp>

mach::Sensor::Sensor(std::string name, std::string pin, std::vector<std::string> settingNames, 
                     std::vector<double> settingValues) : mach::LJDevice::LJDevice(name, pin) {
    this->settingNames = settingNames;
    this->settingValues = settingValues;
}

void mach::Sensor::initialize(mach::LabJack labjack) {
    try {
        int err, errorAddress = INITIAL_ERR_ADDRESS;
        err = LJM_eWriteNames(labjack.getHandle(), int(settingNames.size()),
            util::vectorToChar(settingNames),
            util::vectorToDouble(settingValues),
            &errorAddress
        );
        ErrorCheck(err, "LJM_eWriteNames");
    } catch (...) {
        std::cout << "Labjack Sensor Error: " << boost::current_exception_diagnostic_information() << std::endl;
    }
}

double mach::Sensor::readValue() {
    return -1;
}

void mach::Sensor::print() {
    std::cout << "Sensor: " << name << ", Port: " << port << std::endl;
    std::cout << "Settings: " << std::endl;
    for (int i = 0; i < settingNames.size(); i++) {
        std::cout << settingNames[i] << ": " << settingValues[i] << std::endl;
    }
}