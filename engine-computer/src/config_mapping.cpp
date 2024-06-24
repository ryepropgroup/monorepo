#include "mach/config_mapping.hpp"
#include <map>
#include <vector>
#include <string>
#include <format>
#include <functional>
#include <spdlog/spdlog.h>
// #include "../vendor/labjack/LabJackMModbusMap.h"

namespace mach {

static const std::string RANGE_CONFIG = "RANGE";
static const std::string THERMOCOUPLE_TYPE_CONFIG = "THERMOCOUPLE_TYPE";
static const std::string NEGATIVE_PORT_CONFIG = "NEGATIVE_PORT";

std::map<std::string, std::function<std::vector<std::pair<std::string, double>>(std::string, std::string, std::string)>> transformers;

/* https://support.labjack.com/docs/14-1-1-thermocouple-t7-t8-t-series-datasheet */
static constexpr double LABJACK_CELSIUS = 1.0;
static const std::map<std::string, double> THERM_TYPE_MAPPING = {
    {"E", 20.0}, {"J", 21.0}, {"K", 22.0}, {"R", 23.0}, {"T", 24.0}, 
    {"S", 25.0}, {"N", 27.0}, {"B", 28.0}, {"C", 30.0}
};

std::vector<std::pair<std::string, double>>
transformRange(std::string name, std::string value, std::string port) {
    try {
        return {{{std::format("{}_RANGE", port), std::stod(value)}}};
    } catch (std::invalid_argument err) {
        spdlog::error("Failed to parse range option '{}' for port '{}', ignoring!", value, port);
        return {};
    }
}

std::vector<std::pair<std::string, double>>
transformNegativePort(std::string name, std::string value, std::string port) {
    try {
        if (value.length() <= std::string{"AIN"}.length() || !value.starts_with("AIN")) {
            throw std::invalid_argument("Invalid negative port setting value!");
        }
        double portNumber = std::stod(value.substr(std::string{"AIN"}.length()));
        return {{{std::format("{}_NEGATIVE_CH", port), portNumber}}};
    } catch (std::invalid_argument err) {
        spdlog::error("Failed to parse range option '{}' for port '{}', ignoring!", value, port);
        return {};
    }
}

std::vector<std::pair<std::string, double>> 
transformThermocoupleType(std::string name, std::string value, std::string port) {
    if (!THERM_TYPE_MAPPING.contains(value)) {
        spdlog::error("Failed to parse thermocouple option '{}' for port '{}', ignoring!", value, port);
        return {};
    }
    double type = THERM_TYPE_MAPPING.at(value);
    std::vector<std::pair<std::string, double>> settings;
    // AIN#_EF_INDEX (thermocouple type)
    settings.emplace_back(std::format("{}_EF_INDEX", port), type);
    // AIN#_EF_CONFIG_A (temperature unit)
    settings.emplace_back(std::format("{}_EF_CONFIG_A", port), LABJACK_CELSIUS);
    // AIN#_EF_CONFIG_B (CJC sensor modbus address, set to the on-board sensor)
    settings.emplace_back(std::format("{}_EF_CONFIG_B", port), 60052); // LJM_TEMPERATURE_DEVICE_K_ADDRESS
    // AIN#_EF_CONFIG_D (CJC sensor slope, 1.0 for the on-board sensor)
    settings.emplace_back(std::format("{}_EF_CONFIG_D", port), 1.0);
    // AIN#_EF_CONFIG_E (CJC sensor offset, 0.0 for the on-board sensor)
    settings.emplace_back(std::format("{}_EF_CONFIG_E", port), 0.0);
    return settings;
}

std::vector<std::pair<std::string, double>>
transformFallback(std::string name, std::string value, std::string port) {
    try {
        return {{{name, std::stod(value)}}};
    } catch (std::invalid_argument err) {
        spdlog::error("Failed to parse config option '{}: {}' for port '{}', ignoring!", name, value, port);
        return {};
    }
}

void registerTransformers() {
    // Register "RANGE" option.
    transformers.emplace(RANGE_CONFIG, transformRange);
    // Register "THERMOCOUPLE_TYPE".
    transformers.emplace(THERMOCOUPLE_TYPE_CONFIG, transformThermocoupleType);
    // Register "NEGATIVE_PORT".
    transformers.emplace(NEGATIVE_PORT_CONFIG, transformNegativePort);
}

std::pair<std::vector<std::string>, std::vector<double>>
convertConfigToLabjack(std::vector<std::pair<std::string, std::string>>& config, std::string port) {
    registerTransformers(); // TODO Call this from some sort of init method.
    
    std::vector<std::string> settingNames;
    std::vector<double> settingValues;
    for (std::pair<std::string, std::string> setting : config) {
        std::vector<std::pair<std::string, double>> settings;
        if (transformers.contains(setting.first)) {
            auto transformFunction = transformers.at(setting.first);
            settings = transformFunction(setting.first, setting.second, port);
        } else {
            settings = transformFallback(setting.first, setting.second, port);
        }
        for (std::pair<std::string, double> setting : settings) {
            settingNames.push_back(setting.first);
            settingValues.push_back(setting.second);
        }
    }
    return {settingNames, settingValues};
}

} // mach