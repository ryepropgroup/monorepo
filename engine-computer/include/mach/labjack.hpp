#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace mach {

class Sensor;
class Valve;

class LabJack {
    public:
        LabJack(std::string labjackName);
        ~LabJack();
        LabJack(const LabJack&) = delete; // No copying! >:(
        LabJack& operator=(const LabJack&) = delete;

        void startStreaming();
        bool readStream();
        void addSensor(std::shared_ptr<Sensor> sensor);
        void addValve(std::shared_ptr<Valve> valve);
        std::shared_ptr<Valve> getValve(int dioPin);
        void setHigh(std::string port);
        void setLow(std::string port);
        std::string getName() { return labjackName; }
        void setCJCValue(double value);
        double getCJCValue() { return cjcValue; }
        void connect();
        void reconnect();
        void disconnect();
        void labjackWriteNames(std::pair<std::vector<std::string>, std::vector<double>>& settings);

    private:
        std::string labjackName;
        int handle;
        std::vector<std::shared_ptr<Sensor>> sensors;
        std::unordered_map<int, std::shared_ptr<Valve>> valves;
        std::atomic<double> cjcValue = 0;
        std::mutex labjackMutex;
};

} // namespace mach