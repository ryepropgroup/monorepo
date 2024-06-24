#include <string>
#include <vector>
#include <memory>
#include "mach/sequences/action.hpp"

namespace mach {

class Sequence {
    public:
        Sequence(std::string name) : name(name) {}

        void addAction(std::unique_ptr<Action> action) {
            actions.push_back(std::move(action));
        }

    private:
        std::string name;
        std::vector<std::unique_ptr<Action>> actions;
};

} // namespace mach