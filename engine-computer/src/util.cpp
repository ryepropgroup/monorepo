#include "mach/util.hpp"
#include <thread>
#include <memory>
#include <mutex>

/**
 * @brief Convert a vector of strings to a smart pointer array of C-style strings.
 */
std::unique_ptr<const char*[]> mach::util::vectorToChar(const std::vector<std::string> &stringVector) {
    std::unique_ptr<const char*[]> charPtrArray(new const char*[stringVector.size()]);
    for (size_t i = 0; i < stringVector.size(); i++) {
        charPtrArray[i] = stringVector[i].c_str();
    }
    return charPtrArray;
}
