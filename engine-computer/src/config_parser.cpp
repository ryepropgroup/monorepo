#include <memory>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "mach/labjack.hpp"
#include "mach/device/valve.hpp"
#include "mach/device/sensor.hpp"
#include "mach/config_mapping.hpp"
#include "mach/device/device_manager.hpp"

namespace mach {

void parseLabjackConfig(mach::LabJack &labJack, std::string file) {
    spdlog::info("MACH: Reading labjack devices from config file '{}'.", file);
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

        std::unique_ptr<Valve> valve = std::make_unique<Valve>(LABJACK, name, port);
        valve.get()->setLabjack(labJack);
        deviceManager.addDevice(std::move(valve));
    }

    YAML::Node sensorsNode = config["sensors"];
    for (YAML::const_iterator it = sensorsNode.begin(); it != sensorsNode.end(); it++) {
        const YAML::Node sensorNode = *it;
        std::string name = sensorNode["name"].as<std::string>();
        std::string port = sensorNode["port"].as<std::string>();
        YAML::Node settingsNode = sensorNode["settings"];
        std::vector<std::pair<std::string, std::string>> config;
        for (YAML::const_iterator it = settingsNode.begin(); it != settingsNode.end(); it++) {
            std::string configName = it->first.as<std::string>();
            std::string configValue = it->second.as<std::string>();
            config.emplace_back(configName, configValue);
        }
        std::pair<std::vector<std::string>, std::vector<double>> settings = convertConfigToLabjack(config, port);

        std::unique_ptr<Sensor> sensor = std::make_unique<Sensor>(LABJACK, name, port, settings.first, settings.second);
        sensor.get()->setLabjack(labJack);
        deviceManager.addDevice(std::move(sensor));
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

        std::unique_ptr<Valve> valve = std::make_unique<Valve>(REMOTE, name, port);
        deviceManager.addDevice(std::move(valve));
    }

    YAML::Node sensorsNode = config["sensors"];
    for (YAML::const_iterator it = sensorsNode.begin(); it != sensorsNode.end(); it++) {
        const YAML::Node sensorNode = *it;
        std::string name = sensorNode["name"].as<std::string>();
        std::string port = sensorNode["port"].as<std::string>();
        YAML::Node settingsNode = sensorNode["settings"];
        std::vector<std::pair<std::string, std::string>> config;
        for (YAML::const_iterator it = settingsNode.begin(); it != settingsNode.end(); it++) {
            std::string configName = it->first.as<std::string>();
            std::string configValue = it->second.as<std::string>();
            config.emplace_back(configName, configValue);
        }
        std::pair<std::vector<std::string>, std::vector<double>> settings = convertConfigToLabjack(config, port);

        std::unique_ptr<Sensor> sensor = std::make_unique<Sensor>(REMOTE, name, port, settings.first, settings.second);
        deviceManager.addDevice(std::move(sensor));
    }
    spdlog::info("MACH: Successfully read remote devices from config!");
}

} // namespace mach