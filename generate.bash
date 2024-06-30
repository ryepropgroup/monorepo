#!/bin/bash

# Define the paths to the proto files
PROTO_DIR="./protobuf"
GO_PROJ_DIR="./go/src/protos"
GO_PROJ_DIR_MSG=$GO_PROJ_DIR"/messages"
MAIN_PROTO="$PROTO_DIR/main.proto"
MESSAGES_PROTO="$PROTO_DIR/messages.proto"

# Create directories for the generated code
GO_OUT_DIR="$PROTO_DIR/go"
CPP_OUT_DIR="$PROTO_DIR/cpp"

mkdir -p $GO_PROJ_DIR
mkdir -p $GO_OUT_DIR
mkdir -p $CPP_OUT_DIR

go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
export PATH=$PATH:$(go env GOPATH)/bin
source ~/.bashrc

# Verify installation
    if ! command -v protoc-gen-go &> /dev/null; then
        echo "protoc-gen-go could not be found"
        exit 1
    fi

    if ! command -v protoc-gen-go-grpc &> /dev/null; then
        echo "protoc-gen-go-grpc could not be found"
        exit 1
    fi

PROTOC_GEN_GO=$(which protoc-gen-go)
PROTOC_GEN_GO_GRPC=$(which protoc-gen-go-grpc)
PROTOC_GEN_GRPC=./tools/grpc/grpc_cpp_plugin
# Debugging output to verify paths
echo "PROTOC_GEN_GO path: $PROTOC_GEN_GO"
echo "PROTOC_GEN_GO_GRPC path: $PROTOC_GEN_GO_GRPC"
echo "PROTOC_GEN_GRPC path: $PROTOC_GEN_GRPC"
# Compile the proto files for C++
protoc -I=$PROTO_DIR --cpp_out=$CPP_OUT_DIR --grpc_out=$CPP_OUT_DIR --plugin=protoc-gen-grpc=$PROTOC_GEN_GRPC \
       $MAIN_PROTO $MESSAGES_PROTO

if [ $? -ne 0 ]; then
    echo "Failed to compile the proto files for C++."
    exit 1
fi
# Compile the proto files for Go
protoc -I=$PROTO_DIR --plugin=protoc-gen-go=$PROTOC_GEN_GO --plugin=protoc-gen-go-grpc=$PROTOC_GEN_GO_GRPC \
       --go_out=$GO_PROJ_DIR --go_opt=paths=source_relative \
       --go-grpc_out=$GO_PROJ_DIR --go-grpc_opt=paths=source_relative \
       $MAIN_PROTO $MESSAGES_PROTO
if [ $? -ne 0 ]; then
    echo "Failed to compile the proto files for Go."
    exit 1
    
fi

echo "Successfully compiled the proto files for C++ and Go."
mkdir -p $GO_PROJ_DIR_MSG
mv $GO_PROJ_DIR/messages.pb.go $GO_PROJ_DIR_MSG