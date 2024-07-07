#include "mach/grpc_client.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <spdlog/spdlog.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "messages.pb.h"
#include "mach/sequences/sequence_manager.hpp"
#include "mach/device/device_manager.hpp"
#include "mach/executor.hpp"
#include "mach/thread_safe_queue.hpp"
#include "mach/sequences/open_action.hpp"
#include "mach/sequences/close_action.hpp"

ABSL_FLAG(std::string, target, "100.69.7.77:50051", "Server address");

static void startServerThread(std::string target);

// rpc SensorDataStream (stream SensorData) returns (Response) {}
void mach::ServiceClient::StartSensorDataStream() {
    grpc::ClientContext context;
    mach::proto::Response response;
    std::unique_ptr<grpc::ClientWriter<mach::proto::SensorData>> stream(stub_->SensorDataStream(&context, &response));

    spdlog::info("MACH: Started sensor data stream.");

    while (true) {
        std::pair<long, std::unordered_map<std::string, float>> data = mach::sensorQueue.pop();


        mach::proto::SensorData sensorData;
        sensorData.set_timestamp(data.first);
        for (const auto& [key, value] : data.second) {
            // message SensorData {
            //     int64 timestamp = 1;
            //     map<string, float> values = 2;
            // }
            sensorData.mutable_values()->insert({key, value});
        }
        stream->Write(sensorData);

        // mach::proto::Response response;
        // stream->Read(&response);
    }
}

void mach::ServiceClient::StartCommandStream() {
    // Data we are sending to the server.
    mach::proto::DeviceInformation info;

    // Add existing sequences to device information.
    // TODO: Add existing devices to device information.
    std::vector<std::string> sequences = mach::SequenceManager::getInstance().getSequenceNames();
    for (const auto &sequence : sequences) {
        info.add_sequences(sequence);
    }

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The actual RPC.
    std::unique_ptr<grpc::ClientReader<mach::proto::SequenceCommand>> reader(stub_->CommandStream(&context, info));
    spdlog::info("MACH: Sent device information to server. Listening for commands.");

    mach::proto::SequenceCommand sequence;
    while (reader->Read(&sequence)) {
        spdlog::info("MACH: Received sequence command: {}", sequence.sequence());

        // Commands:
        // valve:V##_s:open
        // valve:V##_sb:close
        // sequence:###
        std::string type = sequence.sequence().substr(0, sequence.sequence().find(":"));
        if (type == "valve") {
            std::string valveName = sequence.sequence().substr(sequence.sequence().find(":") + 1, sequence.sequence().find(":", sequence.sequence().find(":") + 1) - sequence.sequence().find(":") - 1);
            // valveName = valveName.substr(0, valveName.find("_"));
            std::transform(valveName.begin(), valveName.end(), valveName.begin(), ::toupper);

            std::string command = sequence.sequence().substr(sequence.sequence().find(":", sequence.sequence().find(":") + 1) + 1);
            std::shared_ptr<Valve> valve = mach::DeviceManager::getInstance().getValve(valveName);
            if (valve == nullptr) {
                spdlog::warn("MACH: Bad valve command for '{}'", valveName);
            } else if (command == "open") {
                spdlog::info("MACH: Opening valve '{}'", valveName);
                OpenAction action;
                action.setValve(valve);
                action.execute();
            } else if (command == "close") {
                spdlog::info("MACH: Closing valve '{}'", valveName);
                CloseAction action;
                action.setValve(valve);
                action.execute();
            } else {
                spdlog::warn("MACH: Unknown valve command '{}', ignoring!", command);
            }
        } else if (type == "sequence") {
            std::string sequenceName = sequence.sequence().substr(sequence.sequence().find(":") + 1);
            mach::SequenceManager::getInstance().executeSequence(sequenceName);
        } else {
            spdlog::warn("MACH: Unknown command type '{}', ignoring!", type);
        }
    }
    spdlog::info("MACH: GRPC client finished.");
}

void mach::grpcStartClient(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    std::string target_str = absl::GetFlag(FLAGS_target);

    std::thread(startServerThread, target_str).detach();
}

static void startCommandStream(std::string target) {
    spdlog::info("MACH: Starting GRPC client on '{}'", target);
    mach::ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    serviceClient.StartCommandStream();
}

static void startSensorDataStream(std::string target) {
    spdlog::info("MACH: Starting GRPC client on '{}'", target);
    mach::ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    serviceClient.StartSensorDataStream();
}

static void startServerThread(std::string target) {
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    // spdlog::info("MACH: Starting GRPC client on '{}'", target);
    // mach::ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    
    std::thread(startCommandStream, target).detach();
    // std::thread(startSensorDataStream, target).detach(); // TODO
    // startSensorDataStream(serviceClient);
}

void mach::addSensorData(std::unordered_map<std::string, float> data) {
    long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    mach::sensorQueue.push(std::make_pair(timestamp, data));
}