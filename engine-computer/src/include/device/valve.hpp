#pragma once
#include <string>
#include "../labjack.hpp"

namespace mach {

class Valve : public Device<bool> {
    public:
        Valve(std::string name, std::string port);

        bool isOpen();
        void open();
        void close();

        void print() override;
};

} // namespace mach