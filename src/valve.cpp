#include "include/labjack.hpp"
#include "include/valve.hpp"
#include <string>
#include <iostream>

mach::Valve::Valve(std::string name, std::string port) : mach::LJDevice::LJDevice(name, port) {
    this->state = false;
}

bool mach::Valve::isOpen() {
    return state;
}

void mach::Valve::open() {
    labjack->setHigh(port);
}

void mach::Valve::close() {
    labjack->setLow(port);
}

void mach::Valve::print() {
    std::cout << "Valve: " << name << ", Port: " << port << ", State: " << state << std::endl;
}
