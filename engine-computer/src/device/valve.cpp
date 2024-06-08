#include <string>
#include <spdlog/spdlog.h>
#include "include/device/valve.hpp"
#include "include/device/device.hpp"

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
    spdlog::info("{}Valve: {}, Port: {}, State: {}", type == LABJACK ? "Labjack " : "", name, port, isOpen());
}
