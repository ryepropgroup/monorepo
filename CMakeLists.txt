cmake_minimum_required(VERSION 3.10)
project(monorepo)

# Define the output directory
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

# Add the subdirectory containing the Go project
add_subdirectory(go)
# add_subdirectory(engine-computer)
