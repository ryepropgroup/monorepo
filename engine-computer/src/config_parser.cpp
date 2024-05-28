#include <memory>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include "include/labjack.hpp"
#include "include/device/valve.hpp"
#include "include/device/sensor.hpp"
#include "include/config_mapping.hpp"

#define CONFIG_FILE "../config/config.yml"

namespace mach {

void parseConfig(mach::LabJack &labJack) {
    if (!std::filesystem::exists(CONFIG_FILE)) {
        throw std::runtime_error("Config file not found!");
    }
    YAML::Node config = YAML::LoadFile(CONFIG_FILE);
    
    YAML::Node valves = config["valves"];
    for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
        std::string name = it->first.as<std::string>();
        std::string port = it->second.as<std::string>();
        labJack.addDevice(std::make_shared<mach::Valve>(name, port));
    }

    YAML::Node sensors = config["sensors"];
    for (YAML::const_iterator it = sensors.begin(); it != sensors.end(); it++) {
        const YAML::Node sensor = *it;
        std::string name = sensor["name"].as<std::string>();
        std::string port = sensor["port"].as<std::string>();
        YAML::Node settingsNode = sensor["settings"];
        std::vector<std::pair<std::string, std::string>> config;
        for (YAML::const_iterator it = settingsNode.begin(); it != settingsNode.end(); it++) {
            std::string configName = it->first.as<std::string>();
            std::string configValue = it->second.as<std::string>();
            config.emplace_back(configName, configValue);
        }
        std::pair<std::vector<std::string>, std::vector<double>> settings = convertConfigToLabjack(config, port);
        labJack.addDevice(std::make_shared<mach::Sensor>(name, port, settings.first, settings.second));
    }
}

} // namespace mach