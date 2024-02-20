#include <yaml-cpp/yaml.h>
#include "include/labjack.hpp"
#include "include/valve.hpp"
#include "include/sensor.hpp"
#include <memory>

#define CONFIG_FILE "config.yml"

namespace mach::parser {

void parseConfig(mach::LabJack labJack) {
    YAML::Node config = YAML::LoadFile(CONFIG_FILE);
    
    YAML::Node valves = config["valves"];
    for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
        std::string name = it->first.as<std::string>();
        std::string port = it->second.as<std::string>();
        labJack.addDevice(std::move(std::make_shared<mach::Valve>(name, port)));
    }

    YAML::Node sensors = config["sensors"];
    for (YAML::const_iterator it = sensors.begin(); it != sensors.end(); it++) {
        const YAML::Node sensor = *it;
        std::string name = sensor["name"].as<std::string>();
        std::string port = sensor["port"].as<std::string>();
        std::vector<std::string> settingNames;
        std::vector<double> settingValues;
        YAML::Node settings = sensor["settings"];
        for (YAML::const_iterator it = settings.begin(); it != settings.end(); it++) {
            settingNames.push_back(it->first.as<std::string>());
            settingValues.push_back(it->second.as<double>());
        }
        labJack.addDevice(std::move(std::make_shared<mach::Sensor>(name, port, settingNames, settingValues)));
    }
}

}