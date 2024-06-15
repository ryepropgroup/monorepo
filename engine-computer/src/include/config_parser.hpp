#pragma once
#include "labjack.hpp"

namespace mach {

/**
 * Parses the configuration file, adding devices to the global DeviceManager, 
 * and registering devices with the given LabJack.
 */
void parseLabjackConfig(LabJack &labJack, std::string file);

/**
 * Parses the configuration file, adding devices to the global DeviceManager
 * as remote devices (not attached to any LabJack instance).
 */
void parseRemoteConfig(std::string file);

} // namespace mach