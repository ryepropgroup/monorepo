#include "mach/util.hpp"
#include <thread>
#include <memory>

const double* mach::util::vectorToDouble(const std::vector<double> &doubleVector) {
    return doubleVector.data();
}

std::unique_ptr<const char*[]> mach::util::vectorToChar(const std::vector<std::string> &stringVector) {
    std::unique_ptr<const char*[]> charPtrArray(new const char*[stringVector.size()]);
    for (size_t i = 0; i < stringVector.size(); i++) {
        charPtrArray[i] = stringVector[i].c_str();
    }
    return charPtrArray;
}

bool mach::util::sleepOrAbort(std::chrono::milliseconds duration) {
    // TODO Need software abort?
    // std::unique_lock<std::mutex> lock(abort_mutex);
    // abort_cv.wait_for(lock, duration, []() { return abort_flag.load(); });
    // return abort_flag.load();
    std::this_thread::sleep_for(duration);
    return true;
}