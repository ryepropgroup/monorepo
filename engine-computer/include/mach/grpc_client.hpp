#pragma once
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"
#include "mach/thread_safe_queue.hpp"

namespace mach {

class ServiceClient {

public:
    ServiceClient(std::shared_ptr<grpc::Channel> channel) : stub_(mach::proto::EngineComputer::NewStub(channel)) {}

    void StartSensorDataStream();

    void StartCommandStream();

private:
    std::unique_ptr<mach::proto::EngineComputer::Stub> stub_;
};

void grpcStartClient(int argc, char **argv);

void addSensorData(std::unordered_map<std::string, double> data);

} // namespace mach;