#include "mach/device/device_manager.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include "mach/executor.hpp"

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

void mach::DeviceManager::abortAllLabjacks() {
    spdlog::info("MACH: Aborting all sequences.");
    abortFlag = true;
    abortCondition.notify_all();
    mach::Executor::getInstance().executeSequence("abort");
}

void mach::DeviceManager::disconnectAllLabjacks() {
    if (abortFlag) {
        spdlog::info("MACH: Already disconnected, skipping.");
        return;
    }
    spdlog::info("MACH: Aborting all sequences.");
    abortFlag = true;
    abortCondition.notify_all();

    spdlog::info("MACH: Disconnecting all labjacks.");
    for (auto& labJack : labJacks) {
        labJack->disconnect();
    }
}

void mach::DeviceManager::connectAllLabjacks() {
    spdlog::info("MACH: Connecting all labjacks.");
    for (auto& labJack : labJacks) {
        labJack->connect();
    }
}

void mach::DeviceManager::reconnectAllLabjacks() {
    this->disconnectAllLabjacks();
    spdlog::info("MACH: Reconnecting all labjacks.");
    abortFlag = false;
    spdlog::info("MACH: Disabled abort mode.");
    this->connectAllLabjacks();
    this->startAllLabjackStreams();
}

static void readLabjackStreams(std::list<std::shared_ptr<mach::LabJack>>& labJacks) {
    spdlog::info("MACH: Started reading LabJack streams.");
    while (true) { // TODO Abort?
        for (auto& labJack : labJacks) {
            if (!labJack->readStream()) {
                spdlog::info("MACH: Stopped reading all LabJack streams.");
                return;
            }
        }
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

bool mach::DeviceManager::isAborting() {
    return abortFlag;
}

bool mach::DeviceManager::sleepOrAbort(std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(abortMutex);
    abortCondition.wait_for(lock, duration, [this]() { return this->abortFlag.load(); });
    return abortFlag.load();
}
