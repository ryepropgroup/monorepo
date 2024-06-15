#pragma once
#include <vector>
#include <string>

namespace mach::util {

const double* vectorToDouble(const std::vector<double> &doubleVector);

const char** vectorToChar(const std::vector<std::string> &stringVector);

template<typename Rep, typename Period>
bool sleepOrAbort(std::chrono::duration<Rep, Period> duration);

} // namespace mach::util