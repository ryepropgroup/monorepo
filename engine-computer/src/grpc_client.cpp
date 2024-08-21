#include "mach/grpc_client.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include "absl/flags/flag.h"
#include "messages.pb.h"
#include "mach/sequences/sequence_manager.hpp"
#include "mach/executor.hpp"
#include "mach/device/device_manager.hpp"
#include "mach/executor.hpp"
#include "mach/thread_safe_queue.hpp"
#include "mach/sequences/open_action.hpp"
#include "mach/sequences/close_action.hpp"
#include "mach/broadcast.hpp"

ABSL_FLAG(std::string, ip, "localhost:50051", "Server address");

namespace mach {

static ThreadSafeQueue<std::pair<long, std::unordered_map<std::string, double>>> sensorQueue;

void ServiceClient::StartSensorDataStream() {
    grpc::ClientContext context;
    proto::Response response;
    std::unique_ptr<grpc::ClientWriter<proto::SensorData>> stream(stub_->SensorDataStream(&context, &response));

    spdlog::info("MACH: Started sensor data stream.");

    while (true) {
        std::pair<long, std::unordered_map<std::string, double>> data = sensorQueue.pop();
        proto::SensorData sensorData;
        sensorData.set_timestamp(data.first);
        for (const auto& [key, value] : data.second) {
            sensorData.mutable_values()->insert({key, static_cast<float>(value)});
        }
        stream->Write(sensorData);
    }
}

void ServiceClient::StartCommandStream() {
    // Data we are sending to the server.
    proto::DeviceInformation info;

    // Add existing devices and sequences to device information.
    DeviceManager& deviceManager = DeviceManager::getInstance();
    std::vector<std::shared_ptr<Sensor>> sensors = deviceManager.getSensors();
    for (const auto& sensor : sensors) {
        mach::proto::Device device;
        device.set_pin(sensor->getLabJackPin());
        device.set_type(proto::Device_DeviceType_SENSOR);
        device.set_location(proto::Device_DeviceLocation_GSE);
        info.mutable_devices()->insert({sensor->getName(), device});
    }

    std::vector<std::string> sequences = SequenceManager::getInstance().getSequenceNames();
    for (const auto &sequence : sequences) {
        info.add_sequences(sequence);
    }

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The actual RPC.
    std::unique_ptr<grpc::ClientReader<proto::SequenceCommand>> reader(stub_->CommandStream(&context, info));
    spdlog::info("MACH: Sent device information to server. Listening for commands.");

    proto::SequenceCommand sequence;
    while (reader->Read(&sequence)) {
        spdlog::info("MACH: Received sequence command: {}", sequence.sequence());

        // Commands:
        // valve:V##:open
        // valve:V##:close
        // sequence:###
        std::string type = sequence.sequence().substr(0, sequence.sequence().find(":"));
        if (type == "valve") {
            std::string valveName = sequence.sequence().substr(sequence.sequence().find(":") + 1, sequence.sequence().find(":", sequence.sequence().find(":") + 1) - sequence.sequence().find(":") - 1);
            std::transform(valveName.begin(), valveName.end(), valveName.begin(), ::toupper);

            std::string command = sequence.sequence().substr(sequence.sequence().find(":", sequence.sequence().find(":") + 1) + 1);
            std::shared_ptr<Valve> valve = DeviceManager::getInstance().getValve(valveName);
            if (valve == nullptr) {
                spdlog::warn("MACH: Bad valve command for '{}'", valveName);
            } else if (command == "open") {
                valve->open();
            } else if (command == "close") {
                valve->close();
            } else {
                spdlog::warn("MACH: Unknown valve command '{}', ignoring!", command);
            }
        } else if (type == "sequence") {
            std::string sequenceName = sequence.sequence().substr(sequence.sequence().find(":") + 1);
            Executor::getInstance().executeSequence(sequenceName);
        } else {
            spdlog::warn("MACH: Unknown command type '{}', ignoring!", type);
        }
    }
    spdlog::info("MACH: GRPC client finished.");
}

static void startCommandStream(std::string target) {
    spdlog::info("MACH: Starting GRPC client for command stream on '{}'", target);
    ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    serviceClient.StartCommandStream();
}

static void startSensorDataStream(std::string target) {
    spdlog::info("MACH: Starting GRPC client for sensor data on '{}'", target);
    ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    serviceClient.StartSensorDataStream();
}

void grpcStartClient(int argc, char **argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    std::string target_str = absl::GetFlag(FLAGS_ip);

    std::thread(startCommandStream, target_str).detach();
    std::thread(startSensorDataStream, target_str).detach();

    // Start broadcasting the server to the network.
    startBroadcastingServer();
}

void addSensorData(std::unordered_map<std::string, double> data) {
    // Stop data queue from growing indefinitely.
    if (sensorQueue.size() > 100) {
        sensorQueue.pop();
    }
    long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sensorQueue.push(std::make_pair(timestamp, data));
}

} // namespace mach