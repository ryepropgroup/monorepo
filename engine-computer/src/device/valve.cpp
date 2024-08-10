#include "mach/device/valve.hpp"
#include <string>
#include <spdlog/spdlog.h>
#include "mach/device/device.hpp"

bool mach::Valve::isOpen() {
    return getValue();
}

void mach::Valve::open() {
    spdlog::info("MACH: Opening valve '{}'.", this->getName());
    if (this->isNormallyOpen()) {
        labjack->setHigh(port);
    } else {
        labjack->setLow(port);
    }
}

void mach::Valve::close() {
    spdlog::info("MACH: Closing valve '{}'.", this->getName());
    if (this->isNormallyOpen()) {
        labjack->setLow(port);
    } else {
        labjack->setHigh(port);
    }
}

void mach::Valve::print() {
    spdlog::info("{}Valve: {}, Port: {}, State: {}", type == LABJACK ? "Labjack " : "", name, port, isOpen());
}
