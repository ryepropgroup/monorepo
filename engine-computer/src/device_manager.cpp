#include <vector>
#include <unordered_map>
#include "include/labjack.hpp"
#include "include/device/valve.hpp"
#include "include/device/sensor.hpp"
#include "include/device_manager.hpp"


void mach::DeviceManager::addDevice(std::shared_ptr<Valve> valve) {
    valves[valve.get()->getName()] = std::move(valve);
}

void mach::DeviceManager::addDevice(std::shared_ptr<Sensor> sensor) {
    sensors[sensor.get()->getName()] = std::move(sensor);
}

std::shared_ptr<mach::Valve> mach::DeviceManager::getValve(std::string valveName) {
    return valves[valveName];
}

std::shared_ptr<mach::Sensor> mach::DeviceManager::getSensor(std::string sensorName) {
    return sensors[sensorName];
}

void mach::DeviceManager::printDevices() {
    for (auto& device : valves) {
        device.second.get()->print();
    }

    for (auto& device : sensors) {
        device.second.get()->print();
    }
}