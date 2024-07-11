#include "mach/labjack.hpp"
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <algorithm>
#include <spdlog/spdlog.h>
#include "mach/util.hpp"
#include "LabJackM.h"
#include "../vendor/labjack/LJM_Utilities.h"
#include "mach/device/sensor.hpp"
#include "mach/device/valve.hpp"
#include "mach/grpc_client.hpp"
#include "mach/main.hpp"
#include "mach/util.hpp"
#include "mach/device/device_manager.hpp"

#define SCANS_PER_READ 1
#define SCAN_RATE 10.0

static std::unique_ptr<int[]> getScanList(std::vector<std::shared_ptr<mach::Sensor>> sensors);

mach::LabJack::LabJack(std::string labjackName) : labjackName(labjackName), handle(-1) {
    connect();
}

void mach::LabJack::connect() {
    std::lock_guard<std::mutex> lock(labjackMutex);
    spdlog::info("MACH: Connecting to LabJack with name '{}'.", labjackName);
    int err = INITIAL_ERR_ADDRESS;
    if (isDemo()) {
        err = LJM_Open(LJM_dtT7, LJM_ctUSB, LJM_DEMO_MODE, &handle);
    } else {
        err = LJM_Open(LJM_dtT7, LJM_ctUSB, labjackName.c_str(), &handle);
    }
    ErrorCheck(err, "LJM_Open");
}

void mach::LabJack::reconnect() {
    disconnect();
    connect();
}

void mach::LabJack::disconnect() {
    std::lock_guard<std::mutex> lock(labjackMutex);
    spdlog::info("MACH: Disconnecting from LabJack with name '{}'.", labjackName);
    LJM_Close(handle);
    handle = -1;
}

mach::LabJack::~LabJack() {
    disconnect();
}

void mach::LabJack::startStreaming() {
    std::lock_guard<std::mutex> lock(labjackMutex);
    if (handle == -1) {
        spdlog::error("MACH: LabJack '{}' is not connected, cannot start stream!", getName());
        return;
    }
    int err = INITIAL_ERR_ADDRESS;
    err = LJM_eWriteName(handle, "STREAM_TRIGGER_INDEX", 0);
    ErrorCheck(err, "LJM_eWriteName");
    double scanRate = SCAN_RATE;
    std::unique_ptr<int[]> scanList = getScanList(sensors);
    if (!isDemo()) {
        err = LJM_eStreamStart(handle, SCANS_PER_READ, sensors.size(), scanList.get(), &scanRate);
        ErrorCheck(err, "LJM_eStreamStart");
    }
    spdlog::info("MACH: Successfully started stream for labjack '{}' with {} sensors at {}Hz.", getName(), sensors.size(), scanRate);
}

union udouble {
  double d;
  unsigned long long u;
};

bool mach::LabJack::readStream() {
    std::lock_guard<std::mutex> lock(labjackMutex);
    if (handle == -1) {
        spdlog::warn("MACH: LabJack '{}' is no longer connected, stopped reading stream!", getName());
        return false;
    }
    // spdlog::info("MACH: Reading stream for labjack '{}'.", getName()); // TODO
    int deviceBacklog = 0;
    int ljmBacklog = 0;
    int err = INITIAL_ERR_ADDRESS;
    std::unique_ptr<double[]> data(new double[sensors.size() + 1]);
    if (isDemo()) {
        std::fill(data.get(), data.get() + sensors.size() + 1, 69.0);
    } else {
        err = LJM_eStreamRead(handle, data.get(), &deviceBacklog, &ljmBacklog);
        ErrorCheck(err, "LJM_eStreamRead");
    }

    // Note: Set CJC value before other sensors, since they may depend on it.
    setCJCValue(data[sensors.size()]);

    std::unordered_map<std::string, double> state;

    for (int i = 0; i < sensors.size(); i++) {
        // Update state internally and in data packet.
        sensors[i]->updateValue(data[i]);
        state[sensors[i]->getName()] = data[i];
    }

    udouble ljbits{};
    LJM_eReadName(handle, "DIO_STATE", &ljbits.d);
    std::bitset<sizeof(double) * 8> btest(ljbits.u);
    auto bits = btest.to_string();
    auto vals = bits.substr(18, 16);
    std::reverse(vals.begin(), vals.end());

    for (size_t i = 0; i < vals.size(); i++) {
        std::shared_ptr<Valve> valve = getValve(i);
        if (valve == nullptr) {
            continue;
        }
        std::string name = valve->getName();
        char v = vals.at(i);
        int nb = v - '0';
        // Update state internally and in data packet.
        valve->updateValue(nb);
        state[name] = nb;
    }

    // for (auto const& [key, value] : state) {
    //     spdlog::info("{}: {}", key, value);
    // }
    mach::addSensorData(state);
    return true;
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
    valves[valve->getLabJackPin()] = valve;
}

std::shared_ptr<mach::Valve> mach::LabJack::getValve(int dioPin) {
    return valves[dioPin];
}

void mach::LabJack::setHigh(std::string port) {
    std::lock_guard<std::mutex> lock(labjackMutex);
    if (handle == -1) {
        spdlog::warn("MACH: LabJack '{}' is no longer connected, cancelled setting port '{}' high!", getName(), port);
    }
    LJM_eWriteName(handle, port.c_str(), 1);
}

void mach::LabJack::setLow(std::string port) {
    std::lock_guard<std::mutex> lock(labjackMutex);
    if (handle == -1) {
        spdlog::warn("MACH: LabJack '{}' is no longer connected, cancelled setting port '{}' low!", getName(), port);
    }
    LJM_eWriteName(handle, port.c_str(), 0);
}

void mach::LabJack::labjackWriteNames(std::pair<std::vector<std::string>, std::vector<double>>& settings) {
    if (settings.first.size() == 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(labjackMutex);
    if (handle == -1) {
        spdlog::warn("MACH: LabJack '{}' is no longer connected, cancelled writing names!", getName());
    }
    int errorAddress;
    int err = INITIAL_ERR_ADDRESS;
    std::unique_ptr<const char *[]> settingNames = util::vectorToChar(settings.first);
    err = LJM_eWriteNames(handle, int(settings.first.size()), settingNames.get(), settings.second.data(), &errorAddress);
    ErrorCheck(err, "LJM_eWriteNames");
}