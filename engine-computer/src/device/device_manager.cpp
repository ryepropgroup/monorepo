#include "mach/device/device_manager.hpp"
#include <spdlog/spdlog.h>
#include <thread>

static void readLabjackStreams(std::list<std::shared_ptr<mach::LabJack>>& labJacks);

void mach::DeviceManager::addLabJack(std::shared_ptr<LabJack> labJack) {
    labJacks.push_back(labJack);
}

void mach::DeviceManager::addDevice(std::shared_ptr<Valve> valve) {
    if (valves.find(valve.get()->getName()) != valves.end()) {
        spdlog::warn("MACH: Valve '{}' was already registered, skipping it!", valve.get()->getName());
        return;
    }
    valves[valve.get()->getName()] = std::move(valve);
}

void mach::DeviceManager::addDevice(std::shared_ptr<Sensor> sensor) {
    if (sensors.find(sensor.get()->getName()) != sensors.end()) {
        spdlog::warn("MACH: Sensor '{}' was already registered, skipping it!", sensor.get()->getName());
        return;
    }
    sensors[sensor.get()->getName()] = std::move(sensor);
}

void mach::DeviceManager::startAllLabjackStreams() {
    spdlog::info("MACH: Starting all LabJack streams.");
    for (auto& labJack : labJacks) {
        labJack->startStreaming();
    }
    std::thread(readLabjackStreams, std::ref(labJacks)).detach();
}

static void readLabjackStreams(std::list<std::shared_ptr<mach::LabJack>>& labJacks) {
    spdlog::info("MACH: Started reading LabJack streams.");
    while (true) { // TODO Abort?
        for (auto& labJack : labJacks) {
            labJack->readStream();
        }
        // 1 in 10,000,000 chance of printing devices.
        // if (rand() % 10000000 == 0) {
        //     mach::DeviceManager::getInstance().printDevices();
        // }
    }
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