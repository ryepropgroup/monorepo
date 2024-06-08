#include <vector>
#include "include/labjack.hpp"
#include "include/device/valve.hpp"
#include "include/device/sensor.hpp"
#include "include/device_manager.hpp"

void mach::DeviceManager::addDevice(std::unique_ptr<Device<double>> floatDevice) {
    floatDevices.push_back(std::move(floatDevice));
}

void mach::DeviceManager::addDevice(std::unique_ptr<Device<bool>> boolDevice) {
    boolDevices.push_back(std::move(boolDevice));
}

void mach::DeviceManager::printDevices() {
    for (const auto& device : floatDevices) {
        device.get()->print();
    }
    for (const auto& device : boolDevices) {
        device.get()->print();
    }
}