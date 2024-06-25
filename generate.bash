#!/bin/bash

# Define the paths to the proto files
PROTO_DIR="./protobuf"
MAIN_PROTO="$PROTO_DIR/main.proto"
MESSAGES_PROTO="$PROTO_DIR/messages.proto"

# Create directories for the generated code
GO_OUT_DIR="$PROTO_DIR/go"
CPP_OUT_DIR="$PROTO_DIR/proto/cpp"

mkdir -p $GO_OUT_DIR
mkdir -p $CPP_OUT_DIR

# Compile the proto files for Go
protoc -I=$PROTO_DIR --go_out=$GO_OUT_DIR --go_opt=paths=source_relative \
       --go-grpc_out=$GO_OUT_DIR --go-grpc_opt=paths=source_relative \
       $MAIN_PROTO $MESSAGES_PROTO

# Compile the proto files for C++
protoc -I=$PROTO_DIR --cpp_out=$CPP_OUT_DIR --grpc_out=$CPP_OUT_DIR --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
       $MAIN_PROTO $MESSAGES_PROTO

echo "Protobuf files have been successfully compiled for Go and C++."