#include <vector>
#include <string>

namespace mach {
namespace util {

const double* vectorToDouble(const std::vector<double> &doubleVector);

const char** vectorToChar(const std::vector<std::string> &stringVector);

}
}