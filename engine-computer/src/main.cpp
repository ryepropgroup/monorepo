#include <iostream>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include "include/labjack.hpp"
#include "include/config_parser.hpp"
#include "include/device_manager.hpp"

#define LABJACK_CONFIG std::string("../../config/labjack.yml")
#define REMOTE_CONFIG std::string("../../config/remote.yml")

int main() {
    spdlog::info("MACH: Hello World!");

    // One LabJack for now.
    mach::LabJack labJack("LabJack");

    // Read configuration 🤓
    mach::parseLabjackConfig(labJack, LABJACK_CONFIG);
    mach::parseRemoteConfig(REMOTE_CONFIG);

    // Print all devices for debug.
    mach::DeviceManager& deviceManager = mach::DeviceManager::getInstance();
    deviceManager.printDevices();
}