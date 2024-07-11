#pragma once
#include <vector>
#include <string>
#include <chrono>

namespace mach::util {

std::unique_ptr<const char*[]> vectorToChar(const std::vector<std::string> &stringVector);

} // namespace mach::util