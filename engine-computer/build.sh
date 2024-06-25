#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
echo "********** MACH: Configuring the build with CMake **********"
cmake --preset=linux

else
export CC=/opt/homebrew/bin/gcc-14
export CXX=/opt/homebrew/bin/g++-14
cmake --preset=macos
fi

echo "********** MACH: Building the project **********"
cmake --build build 
cp ./build/engine_computer ../build/bin