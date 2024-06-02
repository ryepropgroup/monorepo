cmake_minimum_required(VERSION 3.10)
project(protoServer)

# Set the main Go file
set(GO_MAIN_FILE src/cmd/protoServer.go)

# Ensure the GO_OUTPUT_DIR variable is set
if (NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR is not set. Please set it in the parent CMakeLists.txt.")
endif()

# Ensure the output directory exists
file(MAKE_DIRECTORY ${OUTPUT_DIR})


# Command to build the Go project
add_custom_target(build_go ALL
    COMMAND go build -o ${GO_OUTPUT_DIR}/protoServer ${GO_MAIN_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Go project..."
)

# Command to run the Go project
add_custom_target(run_go
    COMMAND ${GO_OUTPUT_DIR}/protoServer 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Go project..."
)

# Ensure the build_go target is executed before the run_go target
add_dependencies(run_go build_go)
