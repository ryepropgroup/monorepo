#pragma once
#include <string>
#include <vector>
#include <memory>
#include "device/device.hpp"

namespace mach {

    class DeviceManager {
        public:
            static DeviceManager& getInstance() {
                static DeviceManager instance;
                return instance;
            }

            DeviceManager(DeviceManager const&) = delete;
            void operator=(DeviceManager const&) = delete;

            void addDevice(std::unique_ptr<Device<double>> floatDevice);
            void addDevice(std::unique_ptr<Device<bool>> boolDevice);
            void printDevices();
        
        private:
            DeviceManager() {}

            std::vector<std::unique_ptr<Device<double>>> floatDevices;
            std::vector<std::unique_ptr<Device<bool>>> boolDevices;
    };

} // namespace mach