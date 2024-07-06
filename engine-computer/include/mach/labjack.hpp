#pragma once
#include <string>
#include <vector>
#include <memory>

namespace mach {

class Sensor;

class LabJack {
    public:
        LabJack(std::string labjackName);
        ~LabJack();
        LabJack(const LabJack&) = delete; // No copying! >:(
        LabJack& operator=(const LabJack&) = delete;

        int getHandle();
        void startStreaming();
        void readStream();
        void addSensor(std::shared_ptr<Sensor> sensor);
        void setHigh(std::string port);
        void setLow(std::string port);
        std::string getName() { return labjackName; }
        void setCJCValue(double value);
        double getCJCValue() { return cjcValue; }

    private:
        std::string labjackName;
        int handle;
        std::vector<std::shared_ptr<Sensor>> sensors;
        double cjcValue = 0;
};

} // namespace mach