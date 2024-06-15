#include <chrono>
#include <thread>
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

template<typename Rep, typename Period>
bool mach::util::sleepOrAbort(std::chrono::duration<Rep, Period> duration) {
    // TODO Need software abort?
    // std::unique_lock<std::mutex> lock(abort_mutex);
    // abort_cv.wait_for(lock, duration, []() { return abort_flag.load(); });
    // return abort_flag.load();
    std::this_thread::sleep_for(duration);
    return false;
}