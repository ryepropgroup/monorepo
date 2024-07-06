#pragma once
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

namespace mach {

class ServiceClient {

public:
    ServiceClient(std::shared_ptr<grpc::Channel> channel) : stub_(mach::proto::EngineComputer::NewStub(channel)) {}

    void StartCommandStream();

private:
    std::unique_ptr<mach::proto::EngineComputer::Stub> stub_;
};

void grpcStartClient(int argc, char **argv);

} // namespace mach;