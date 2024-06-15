#include <iostream>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include "include/labjack.hpp"
#include "include/config_parser.hpp"
#include "include/device_manager.hpp"
#include "include/sequences/action_factory.hpp"
#include "include/sequences/open_action.hpp"
#include "include/device/device.hpp"
#include "include/sequences/sleep_action.hpp"
#include "include/sequences/close_action.hpp"

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

    // Register sequence actions.
    mach::ActionFactory& actionFactory = mach::ActionFactory::getInstance();
    actionFactory.registerAction<mach::OpenAction>("OPEN");
    actionFactory.registerAction<mach::CloseAction>("CLOSE");
    actionFactory.registerAction<mach::SleepAction>("SLEEP");
}