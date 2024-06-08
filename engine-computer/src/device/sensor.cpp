#include "include/device/sensor.hpp"
#include "include/util.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <spdlog/spdlog.h>
#include "LabJackM.h"
#include <boost/exception/diagnostic_information.hpp>
// #include "../vendor/labjack/LJM_Utilities.h"

void mach::Sensor::setLabjack(mach::LabJack& labjack) {
    // TODO Proper error handling with LabJack.
    try {
        int err, errorAddress = -2; // INITIAL_ERR_ADDRESS;
        err = LJM_eWriteNames(labjack.getHandle(), 
            int(settingNames.size()),
            util::vectorToChar(settingNames),
            util::vectorToDouble(settingValues),
            &errorAddress
        );
        // ErrorCheck(err, "LJM_eWriteNames");
    } catch (...) {
        spdlog::error("Labjack Sensor Error: {}!", boost::current_exception_diagnostic_information());
    }
}

void mach::Sensor::print() {
    spdlog::info("{}Sensor: {}, Port: {}, Value: {}", type == LABJACK ? "Labjack " : "", name, port, getValue());
    for (int i = 0; i < settingNames.size(); i++) {
        spdlog::info("\tSetting: {}, Value: {}", settingNames[i], settingValues[i]);
    }
}