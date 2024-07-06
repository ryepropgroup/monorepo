#include "mach/grpc_client.hpp"
#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "messages.pb.h"
#include "mach/sequences/sequence_manager.hpp"
#include "mach/device/device_manager.hpp"
#include "mach/executor.hpp"

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

static void startServerThread(std::string target);

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
        mach::Executor::getInstance().executeSequence(sequence.sequence());
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

static void startServerThread(std::string target) {
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    spdlog::info("MACH: Starting GRPC client on '{}'", target);
    mach::ServiceClient serviceClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    serviceClient.StartCommandStream();
}