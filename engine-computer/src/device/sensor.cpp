#include "mach/device/sensor.hpp"
#include "mach/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>
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

/* https://support.labjack.com/docs/14-1-1-thermocouple-t7-t8-t-series-datasheet */
static constexpr double LABJACK_CELSIUS = 1.0;
static const std::map<std::string, double> THERM_TYPE_MAPPING = {
    {"E", 20.0}, {"J", 21.0}, {"K", 22.0}, {"R", 23.0}, {"T", 24.0}, 
    {"S", 25.0}, {"N", 27.0}, {"B", 28.0}, {"C", 30.0}
};

void mach::Sensor::setThermocoupleType(char type) {
    thermocoupleType = type;
    // AIN#_EF_INDEX (thermocouple type)
    labJackSettings.first.push_back(std::format("{}_EF_INDEX", port));
    labJackSettings.second.push_back(THERM_TYPE_MAPPING.at(std::string{type}));
    // AIN#_EF_CONFIG_A (temperature unit)
    labJackSettings.first.push_back(std::format("{}_EF_CONFIG_A", port));
    labJackSettings.second.push_back(LABJACK_CELSIUS);
    // AIN#_EF_CONFIG_B (CJC sensor modbus address, set to the on-board sensor)
    labJackSettings.first.push_back(std::format("{}_EF_CONFIG_B", port));
    labJackSettings.second.push_back(60052);
    // AIN#_EF_CONFIG_D (CJC sensor slope, 1.0 for the on-board sensor)
    labJackSettings.first.push_back(std::format("{}_EF_CONFIG_D", port));
    labJackSettings.second.push_back(1.0);
    // AIN#_EF_CONFIG_E (CJC sensor offset, 0.0 for the on-board sensor)
    labJackSettings.first.push_back(std::format("{}_EF_CONFIG_E", port));
    labJackSettings.second.push_back(0.0);
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
    // if (thermocoupleType != '\0') {
    //     if (labjack == nullptr) {
    //         spdlog::error("Sensor '{}' is a thermocouple, but LabJack is not set!", name);
    //         return;
    //     }
    //     int err = INITIAL_ERR_ADDRESS;
    //     if (isDemo()) {
    //         newValue = 69.0 + 273.15;
    //     } 
    //     else {
    //         err = LJM_TCVoltsToTemp(LJM_ttK, value, labjack->getCJCValue(), &newValue);
    //         if (err != LJME_NOERROR) {
    //             char errorString[LJM_MAX_NAME_SIZE];
    //             LJM_ErrorToString(err, errorString);
    //             spdlog::warn("Error converting thermocouple voltage to temperature: {}", errorString);
    //             return;
    //         }
    //     }
    //     newValue -= 273.15;
    // } else 
    if (multiplier != 1.0) {
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