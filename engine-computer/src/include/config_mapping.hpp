#pragma once

#include <vector>
#include <string>

namespace mach {

/**
 * Given the MACH config options for a single device (string->string),
 * converts it to a pair of vectors representing labjack settings.
 * 
 * @param config The MACH config.
 * @param port The port corresponding to the device.
 * @return Labjack settings, in the format (settingNames, settingValues).
*/
std::pair<std::vector<std::string>, std::vector<double>>
convertConfigToLabjack(std::vector<std::pair<std::string, std::string>>& config, std::string port);

} // namespace mach