#include "../include/device/valve.hpp"
#include <string>
#include <iostream>

mach::Valve::Valve(std::string name, std::string port) : mach::Device<bool>::Device(name, port) {
    this->value = false;
}

bool mach::Valve::isOpen() {
    return getValue();
}

void mach::Valve::open() {
    labjack->setHigh(port);
}

void mach::Valve::close() {
    labjack->setLow(port);
}

void mach::Valve::print() {
    std::cout << "Valve: " << name << ", Port: " << port << ", State: " << isOpen() << std::endl;
}
