#pragma once
#include <string>
#include "labjack.hpp"

namespace mach {

class Valve : public LJDevice {
    public:
        Valve(std::string name, std::string port);

        bool isOpen();
        void open();
        void close();

        void print() override;
    
    private:
        bool state;
};

}