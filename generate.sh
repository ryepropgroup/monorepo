#!/bin/bash

# Define the paths to the proto files
PROTO_DIR="./protobuf"
MAIN_PROTO="$PROTO_DIR/service.proto"
MESSAGES_PROTO="$PROTO_DIR/messages.proto"

# Create directories for the generated code
GO_PROJ_DIR="./go/src/protos"
GO_PROJ_DIR_MSG=$GO_PROJ_DIR"/messages"
CPP_OUT_DIR="./engine-computer/generated"
PYTHON_OUT_DIR="./gui/rpcSimulator"

mkdir -p $GO_PROJ_DIR
mkdir -p $GO_PROJ_DIR_MSG
mkdir -p $CPP_OUT_DIR

go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
export PATH=$PATH:$(go env GOPATH)/bin
source ~/.bashrc

source ./gui/venv/bin/activate
python -m grpc_tools.protoc -I=protobuf --python_out=$PYTHON_OUT_DIR --grpc_python_out=$PYTHON_OUT_DIR $MAIN_PROTO
python -m grpc_tools.protoc -I=protobuf --python_out=$PYTHON_OUT_DIR $MESSAGES_PROTO

# Verify installation
if ! command -v protoc-gen-go &> /dev/null; then
    echo "protoc-gen-go could not be found"
    exit 1
fi

if ! command -v protoc-gen-go-grpc &> /dev/null; then
    echo "protoc-gen-go-grpc could not be found"
    exit 1
fi

PROTOC=./tools/protobuf/protoc
PROTOC_GEN_GRPC=./tools/grpc/grpc_cpp_plugin
PROTOC_GEN_GO=$(which protoc-gen-go)
PROTOC_GEN_GO_GRPC=$(which protoc-gen-go-grpc)

# Add .exe to paths for windows 👁️👄👁️
if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    PROTOC=$PROTOC".exe"
    PROTOC_GEN_GRPC=$PROTOC_GEN_GRPC".exe"
    PROTOC_GEN_GO=$PROTOC_GEN_GO".exe"
    PROTOC_GEN_GO_GRPC=$PROTOC_GEN_GO_GRPC".exe"
fi

# Debugging output to verify paths
echo "PROTOC_GEN_GO path: $PROTOC_GEN_GO"
echo "PROTOC_GEN_GO_GRPC path: $PROTOC_GEN_GO_GRPC"
echo "PROTOC_GEN_GRPC path: $PROTOC_GEN_GRPC"
# Compile the proto files for C++
$PROTOC -I=$PROTO_DIR --cpp_out=$CPP_OUT_DIR --grpc_out=$CPP_OUT_DIR --plugin=protoc-gen-grpc=$PROTOC_GEN_GRPC \
        $MAIN_PROTO $MESSAGES_PROTO

if [ $? -ne 0 ]; then
    echo "Failed to compile the proto files for C++."
    exit 1
fi
# Compile the proto files for Go
$PROTOC -I=$PROTO_DIR --plugin=protoc-gen-go=$PROTOC_GEN_GO --plugin=protoc-gen-go-grpc=$PROTOC_GEN_GO_GRPC \
       --go_out=$GO_PROJ_DIR --go_opt=paths=source_relative \
       --go-grpc_out=$GO_PROJ_DIR --go-grpc_opt=paths=source_relative \
       $MAIN_PROTO $MESSAGES_PROTO
if [ $? -ne 0 ]; then
    echo "Failed to compile the proto files for Go."
    exit 1
    
fi

echo "Successfully compiled the proto files for C++ and Go."
mv $GO_PROJ_DIR/messages.pb.go $GO_PROJ_DIR_MSG