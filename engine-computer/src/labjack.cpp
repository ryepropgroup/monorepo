#include "mach/labjack.hpp"
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include "mach/util.hpp"
#include "LabJackM.h"
#include "../vendor/labjack/LJM_Utilities.h"
#include "mach/device/sensor.hpp"
#include "mach/device/valve.hpp"
#include "mach/grpc_client.hpp"

#define SCANS_PER_READ 1
#define SCAN_RATE 10.0

static std::unique_ptr<int[]> getScanList(std::vector<std::shared_ptr<mach::Sensor>> sensors);

mach::LabJack::LabJack(std::string labjackName) : labjackName(labjackName), handle(-1) {
    spdlog::info("MACH: Connecting to LabJack with name '{}'.", labjackName);
    int err = INITIAL_ERR_ADDRESS;
    // err = LJM_Open(LJM_dtT7, LJM_ctUSB, labjackName.c_str(), &handle);
    // err = LJM_Open(LJM_dtT7, LJM_ctUSB, 0, &handle);
    err = LJM_Open(LJM_dtT7, LJM_ctUSB, LJM_DEMO_MODE, &handle); // TODO
    ErrorCheck(err, "LJM_Open");
}

mach::LabJack::~LabJack() {
    LJM_Close(handle);
}

void mach::LabJack::startStreaming() {
    int err = INITIAL_ERR_ADDRESS;
    err = LJM_eWriteName(handle, "STREAM_TRIGGER_INDEX", 0);
    ErrorCheck(err, "LJM_eWriteName");
    // TODO Digital states...
    double scanRate = SCAN_RATE;
    std::unique_ptr<int[]> scanList = getScanList(sensors);
    // err = LJM_eStreamStart(handle, SCANS_PER_READ, sensors.size(), scanList.get(), &scanRate);
    // ErrorCheck(err, "LJM_eStreamStart"); // TODO
    spdlog::info("MACH: Successfully started stream for labjack '{}' with {} sensors at {}Hz.", getName(), sensors.size(), scanRate);
}

union udouble {
  double d;
  unsigned long long u;
};

void mach::LabJack::readStream() {
    int deviceBacklog = 0;
    int ljmBacklog = 0;
    int err = INITIAL_ERR_ADDRESS;
    std::unique_ptr<double[]> data(new double[sensors.size() + 1]);
    // err = LJM_eStreamRead(handle, data.get(), &deviceBacklog, &ljmBacklog);
    // ErrorCheck(err, "LJM_eStreamRead"); // TODO

    // Note: Set CJC value before other sensors, since they may depend on it.
    setCJCValue(data[sensors.size()]);

    std::unordered_map<std::string, float> state;

    for (int i = 0; i < sensors.size(); i++) {
        // sensors[i]->updateValue(data[i]); TODO
        state[sensors[i]->getName()] = data[i];
    }

    udouble ljbits{};
    LJM_eReadName(handle, "DIO_STATE", &ljbits.d);
    std::bitset<sizeof(double) * 8> btest(ljbits.u);
    auto bits = btest.to_string();
    auto vals = bits.substr(18, 16);
    std::reverse(vals.begin(), vals.end());
    
    // TODO Update valve states.

    for (size_t i = 0; i < vals.size(); i++) {
        std::shared_ptr<Valve> valve = getValve(i);
        if (valve == nullptr) {
            // spdlog::warn("MACH: Valve at DIO pin {} not found!", i);
            continue;
        }

        std::string name = valve->getName();

        char v = vals.at(i);
        int nb = v - '0';
        state[name] = nb;
    }

    mach::addSensorData(state);
}

static std::unique_ptr<int[]> getScanList(std::vector<std::shared_ptr<mach::Sensor>> sensors) {
    std::unique_ptr<int[]> scanList(new int[sensors.size() + 1]);
    for (int i = 0; i < sensors.size(); i++) {
        scanList[i] = sensors[i]->getLabJackPin() * 2; // Why is this multiplied by 2? 
    }
    // Note: This is the internal CJC sensor (AIN14 = 28), always scan it.
    scanList[sensors.size()] = 28;
    return scanList;
}

void mach::LabJack::setCJCValue(double value) {
    this->cjcValue = value * -92.6 + 467.6;
}

void mach::LabJack::addSensor(std::shared_ptr<Sensor> sensor) {
    sensors.push_back(sensor);
}

void mach::LabJack::addValve(std::shared_ptr<Valve> valve) {
    // spdlog::info("MACH: Adding valve '{}' with pin {} to LabJack '{}'.", valve->getName(), valve->getLabJackPin(), getName());
    valves[valve->getLabJackPin()] = valve;
}

std::shared_ptr<mach::Valve> mach::LabJack::getValve(int dioPin) {
    return valves[dioPin];
}

int mach::LabJack::getHandle() {
    return handle;
}

void mach::LabJack::setHigh(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 1);
}

void mach::LabJack::setLow(std::string port) {
    LJM_eWriteName(handle, port.c_str(), 0);
}