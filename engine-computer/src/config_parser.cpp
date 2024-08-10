#include <memory>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "mach/labjack.hpp"
#include "mach/device/valve.hpp"
#include "mach/device/sensor.hpp"
#include "mach/device/device_manager.hpp"

namespace mach {

static void parseLabjackConfig(std::string file);

void parseAllLabjacks(const std::string labjacksFolder) {
    if (!std::filesystem::exists(labjacksFolder)) {
        spdlog::error("MACH: Labjacks folder not found at '{}'!", labjacksFolder);
        throw std::runtime_error("Labjacks folder file not found!");
    }
    spdlog::info("MACH: Reading labjacks from folder '{}'.", labjacksFolder);

    std::filesystem::directory_iterator end_itr;
    for (std::filesystem::directory_iterator itr(labjacksFolder); itr != end_itr; ++itr) {
        if (std::filesystem::is_regular_file(itr->path())) {
            std::string file = itr->path().string();
            if (file.ends_with(".yml")) {
                parseLabjackConfig(file);
            }
        }
    }
}

static void parseLabjackConfig(std::string file) {
    spdlog::info("MACH: Reading labjack devices from config file '{}'.", file);
    if (!std::filesystem::exists(file)) {
        spdlog::error("MACH: Config file '{}' not found!", file);
        throw std::runtime_error("Config file not found!");
    }
    YAML::Node config = YAML::LoadFile(file);

    // Register a new LabJack device in the device manager.
    std::string name = config["name"].as<std::string>();
    std::shared_ptr<LabJack> labJack = std::make_shared<LabJack>(name);
    DeviceManager& deviceManager = DeviceManager::getInstance();
    deviceManager.addLabJack(labJack);

    spdlog::info("MACH: Reading valves from config file '{}'.", name);
    
    YAML::Node valves = config["valves"];
    for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
        std::string name = it->first.as<std::string>();
        std::string port = it->second.as<std::string>();

        std::shared_ptr<Valve> valve = std::make_shared<Valve>(LABJACK, name, port);
        valve->setLabjack(labJack);
        labJack->addValve(valve);
        deviceManager.addDevice(valve);
    }

    spdlog::info("MACH: Reading sensors from config file '{}'.", name);

    YAML::Node sensorsNode = config["sensors"];
    for (YAML::const_iterator it = sensorsNode.begin(); it != sensorsNode.end(); it++) {
        const YAML::Node sensorNode = *it;
        std::string name = sensorNode["name"].as<std::string>();
        std::string port = sensorNode["port"].as<std::string>();
        std::shared_ptr<Sensor> sensor = std::make_shared<Sensor>(LABJACK, name, port);

        if (sensorNode["range"]) {
            sensor->setRange(sensorNode["range"].as<double>());
        }
        if (sensorNode["multiplier"]) {
            sensor->setMultiplier(sensorNode["multiplier"].as<double>());
        }
        if (sensorNode["negative_channel"]) {
            sensor->setNegativeChannel(sensorNode["negative_channel"].as<std::string>());
        }
        if (sensorNode["thermocouple_type"]) {
            sensor->setThermocoupleType(sensorNode["thermocouple_type"].as<std::string>());
        }

        sensor->setLabjack(labJack);
        labJack->addSensor(sensor);
        deviceManager.addDevice(sensor);
    }
    spdlog::info("MACH: Successfully read labjack devices from config!");
}

void parseRemoteConfig(std::string file) {
    spdlog::info("MACH: Reading remote devices from config file '{}'.", file);
    if (!std::filesystem::exists(file)) {
        spdlog::error("MACH: Config file '{}' not found!", file);
        throw std::runtime_error("Config file not found!");
    }
    YAML::Node config = YAML::LoadFile(file);

    DeviceManager& deviceManager = DeviceManager::getInstance();
    
    YAML::Node valves = config["valves"];
    for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
        std::string name = it->first.as<std::string>();
        std::string port = it->second.as<std::string>();

        std::shared_ptr<Valve> valve = std::make_shared<Valve>(REMOTE, name, port);
        deviceManager.addDevice(valve);
    }

    YAML::Node sensorsNode = config["sensors"];
    for (YAML::const_iterator it = sensorsNode.begin(); it != sensorsNode.end(); it++) {
        const YAML::Node sensorNode = *it;
        std::string name = sensorNode["name"].as<std::string>();
        std::string port = sensorNode["port"].as<std::string>();
        std::shared_ptr<Sensor> sensor = std::make_shared<Sensor>(REMOTE, name, port);
        deviceManager.addDevice(sensor);
    }
    spdlog::info("MACH: Successfully read remote devices from config!");
}

} // namespace mach