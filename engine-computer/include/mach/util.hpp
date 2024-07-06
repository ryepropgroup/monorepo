#pragma once
#include <vector>
#include <string>
#include <chrono>

namespace mach::util {

const double* vectorToDouble(const std::vector<double> &doubleVector);

std::unique_ptr<const char*[]> vectorToChar(const std::vector<std::string> &stringVector);

bool sleepOrAbort(std::chrono::milliseconds duration);

} // namespace mach::util