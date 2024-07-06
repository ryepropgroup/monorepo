#include "mach/device/sensor.hpp"
#include "mach/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <spdlog/spdlog.h>
#include "LabJackM.h"
#include "../../vendor/labjack/LJM_Utilities.h"
#include "../../vendor/labjack/LabJackM.h"

mach::Sensor::Sensor(DeviceType type, std::string name, std::string port) : 
        Device(type, name, port), labJackPin(-1) {
    this->value = 0.0;

    if (type == LABJACK) {
        // Convert AIN# to an integer.
        if (port.length() <= std::string{"AIN"}.length() || !port.starts_with("AIN")) {
            spdlog::error("Invalid sensor port value: {}!", port);
            throw std::invalid_argument("Invalid sensor port value!");
        }
        this->labJackPin = std::stoi(port.substr(std::string{"AIN"}.length()));
    }
}

void mach::Sensor::setLabjack(std::shared_ptr<LabJack> labjack) {
    if (labJackSettings.first.size() == 0) {
        return;
    }
    int errorAddress;
    int err = INITIAL_ERR_ADDRESS;
    std::unique_ptr<const char *[]> settingNames = util::vectorToChar(labJackSettings.first);
    err = LJM_eWriteNames(labjack->getHandle(), int(labJackSettings.first.size()),
        settingNames.get(), util::vectorToDouble(labJackSettings.second), &errorAddress);
    ErrorCheck(err, "LJM_eWriteNames");
}

void mach::Sensor::setThermocoupleType(char type) {
    thermocoupleType = type;
}

void mach::Sensor::setRange(double range) {
    this->range = range;
    labJackSettings.first.push_back(std::format("{}_RANGE", port));
    labJackSettings.second.push_back(range);
}

void mach::Sensor::setMultiplier(double multiplier) {
    this->multiplier = multiplier;
}

void mach::Sensor::setNegativeChannel(std::string channel) {
    negativeChannel = channel;
    labJackSettings.first.push_back(std::format("{}_NEGATIVE_CH", port));
    labJackSettings.second.push_back(std::stod(channel.substr(std::string("AIN").length())));
}

void mach::Sensor::updateValue(double value) {
    if (thermocoupleType != 0) {
        double cjcValue = labjack->getCJCValue();
        int err = INITIAL_ERR_ADDRESS;
        double newValue;
        err = LJM_TCVoltsToTemp(LJM_ttK, value, cjcValue, &newValue);
        ErrorCheck(err, "LJM_TCVoltsToTemp");
        this->value = newValue - 273.15;

    } else if (multiplier != 1.0) {
        this->value = value * multiplier;

    } else {
        this->value = value;
    }
}

void mach::Sensor::print() {
    spdlog::info("{}Sensor: {}, Port: {}, Value: {}", type == LABJACK ? "Labjack " : "", name, port, getValue());
    for (std::size_t i = 0; i < labJackSettings.first.size(); i++) {
        spdlog::info("\tSetting: {}, Value: {}", labJackSettings.first[i], labJackSettings.second[i]);
    }
}