#include "include/labjack_manager.hpp"
#include "include/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "LabJackM.h"
#include "LJM_Utilities.h"
#include <boost/exception/diagnostic_information.hpp>

mach::LabJack::LabJack(std::string labjackName) {
    LJM_Open(LJM_dtT7, LJM_ctUSB, labjackName.c_str(), &handle);
}

mach::LabJack::~LabJack() {
    LJM_Close(handle);
}

void mach::LabJack::addValve(Valve valve) {
    valves.push_back(valve);
}

void mach::LabJack::addSensor(Sensor sensor) {
    try {
        int err, errorAddress = INITIAL_ERR_ADDRESS;
        err = LJM_eWriteNames(handle, int(sensor.settingNames.size()),
            util::vectorToChar(sensor.settingNames),
            util::vectorToDouble(sensor.settingValues),
            &errorAddress
        );
        ErrorCheck(err, "LJM_eWriteNames");
        sensors.push_back(sensor);
    } catch (...) {
        std::cout << "Labjack Error: " << boost::current_exception_diagnostic_information() << std::endl;
    }
}

void mach::LabJack::setHigh(std::string name) {
    LJM_eWriteName(handle, name.c_str(), 1);
}

void mach::LabJack::setLow(std::string name) {
    LJM_eWriteName(handle, name.c_str(), 0);
}
    
mach::Valve::Valve(std::string name, std::string pin) {
    this->name = name;
    this->pin = pin;
}

void mach::Valve::addToLabjack(LabJack labjack) {
    labjack.addValve(*this);
}

bool mach::Valve::getOpenState() {
    return state;
}

void mach::Valve::open() {
    labjack->setHigh(pin);
}

void mach::Valve::close() {
    labjack->setLow(pin);
}

mach::Sensor::Sensor(std::string name, std::string pin, std::vector<std::string> settingNames, std::vector<double> settingValues) {
    this->name = name;
    this->pin = pin;
    this->settingNames = settingNames;
    this->settingValues = settingValues;
}

void mach::Sensor::addToLabjack(LabJack labjack) {
    labjack.addSensor(*this);
}

double mach::Sensor::read() {
    // TODO
}
