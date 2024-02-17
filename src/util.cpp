#include "include/util.hpp"

const double* mach::util::vectorToDouble(const std::vector<double> &doubleVector) {
    return doubleVector.data();
}

const char** mach::util::vectorToChar(const std::vector<std::string> &stringVector) {
    const char **charPtrArray = new const char *[stringVector.size()];
    for (size_t i = 0; i < stringVector.size(); i++) {
        charPtrArray[i] = stringVector[i].c_str();
    }
    return charPtrArray;
}