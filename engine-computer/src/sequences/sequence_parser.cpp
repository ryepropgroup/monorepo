#include <memory>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include "mach/labjack.hpp"
#include "mach/device/valve.hpp"
#include "mach/device/sensor.hpp"
#include "mach/config_mapping.hpp"

// #define SEQUENCES_FOLDER "../config/sequences/"

// namespace mach {

// void parseSequences(LabJack &labJack) {
    // if (!std::filesystem::exists(SEQUENCES_FOLDER)) {
    //     throw std::runtime_error("Sequences folder file not found!");
    // }
    // std::filesystem::directory_iterator end_itr;
    // for (std::filesystem::directory_iterator itr(SEQUENCES_FOLDER); itr != end_itr; ++itr) {
    //     if (std::filesystem::is_regular_file(itr->path())) {
    //         std::string file = itr->path().string();
    //         if (file.find(".yml") != std::string::npos) {
    //             parseSequence(labJack, file);
    //         }
    //     }
    // }
// }

// void parseSequence(LabJack &labJack, std::string file) {
    // YAML::Node config = YAML::LoadFile(file);
    
    // YAML::Node valves = config["valves"];
    // for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
    //     std::string name = it->first.as<std::string>();
    //     std::string port = it->second.as<std::string>();
    //     labJack.addDevice(std::make_shared<mach::Valve>(name, port));
    // }

    // YAML::Node sensors = config["sensors"];
    // for (YAML::const_iterator it = sensors.begin(); it != sensors.end(); it++) {
    //     const YAML::Node sensor = *it;
    //     std::string name = sensor["name"].as<std::string>();
    //     std::string port = sensor["port"].as<std::string>();
    //     YAML::Node settingsNode = sensor["settings"];
    //     std::vector<std::pair<std::string, std::string>> config;
    //     for (YAML::const_iterator it = settingsNode.begin(); it != settingsNode.end(); it++) {
    //         std::string configName = it->first.as<std::string>();
    //         std::string configValue = it->second.as<std::string>();
    //         config.emplace_back(configName, configValue);
    //     }
    //     std::pair<std::vector<std::string>, std::vector<double>> settings = convertConfigToLabjack(config, port);
    //     labJack.addDevice(std::make_shared<mach::Sensor>(name, port, settings.first, settings.second));
    // }
// }

// } // namespace mach