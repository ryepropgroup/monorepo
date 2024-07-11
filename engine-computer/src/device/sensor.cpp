#include "mach/device/sensor.hpp"
#include "mach/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <spdlog/spdlog.h>
#include "LabJackM.h"
#include "../../vendor/labjack/LJM_Utilities.h"
#include "../../vendor/labjack/LabJackM.h"
#include "mach/main.hpp"

mach::Sensor::Sensor(DeviceType type, std::string name, std::string port) : 
        Device(type, name, port) {
    this->updateValue(0.0);
}

void mach::Sensor::setLabjack(std::shared_ptr<LabJack> labjack) {
    this->labjack = labjack;
    labjack->labjackWriteNames(labJackSettings);
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
    double newValue = value;
    if (thermocoupleType != '\0') {
        if (labjack == nullptr) {
            spdlog::error("Sensor '{}' is a thermocouple, but LabJack is not set!", name);
            return;
        }
        int err = INITIAL_ERR_ADDRESS;
        if (isDemo()) {
            newValue = 69.0 + 273.15;
        } else {
            err = LJM_TCVoltsToTemp(LJM_ttK, value, labjack->getCJCValue(), &newValue);
            if (err != LJME_NOERROR) {
                char errorString[LJM_MAX_NAME_SIZE];
                LJM_ErrorToString(err, errorString);
                spdlog::warn("Error converting thermocouple voltage to temperature: {}", errorString);
                return;
            }
        }
        newValue -= 273.15;
    } else if (multiplier != 1.0) {
        newValue = value * multiplier;
    }
    mach::Device<double>::updateValue(newValue);
    // spdlog::info("Sensor '{}' updated to value: {}", name, this->getValue()); // TODO
}

void mach::Sensor::print() {
    spdlog::info("{}Sensor: {}, Port: {}, Value: {}", type == LABJACK ? "Labjack " : "", name, port, getValue());
    for (std::size_t i = 0; i < labJackSettings.first.size(); i++) {
        spdlog::info("\tSetting: {}, Value: {}", labJackSettings.first[i], labJackSettings.second[i]);
    }
}