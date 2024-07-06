#pragma once
#include "labjack.hpp"

namespace mach {

/**
 * @brief Parses the configuration file, registering a new labjack, 
 * and adding its devices to the global DeviceManager.
 */
void parseAllLabjacks(std::string file);

/**
 * @brief Parses the configuration file, adding devices to the global 
 * DeviceManager as remote devices (not attached to any LabJack instance).
 */
void parseRemoteConfig(std::string file);

} // namespace mach