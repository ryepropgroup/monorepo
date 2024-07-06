#!/bin/bash

export VCPKG_BUILD_TYPE=release

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    cmd.exe /c .configurevs.bat

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "********** MACH: Configuring the build with CMake **********"
    cmake engine-computer --preset=linux

else
    export CC=/opt/homebrew/bin/gcc-14
    export CXX=/opt/homebrew/bin/g++-14
    echo "********** MACH: Configuring the build with CMake **********"
    cmake engine-computer --preset=macos
fi

TOOLS_DIR="./tools"

if [ ! -d "$TOOLS_DIR" ]; then
    mkdir -p "$TOOLS_DIR"
    echo "Directory created: $TOOLS_DIR"
else
    echo "Directory already exists: $TOOLS_DIR"
    rm -rf "$TOOLS_DIR"
    mkdir -p "$TOOLS_DIR"
fi

case $(uname -s) in
    Linux*)     OSPATH=x64-linux;;
    Darwin*)    OSPATH=arm64-osx;;
    *)          OSPATH=x64-windows;;
esac

cp -r "./engine-computer/build/vcpkg_installed/$OSPATH/tools" "./"
if [ $? -eq 0 ]; then
    echo "Vcpkg tools copied to $TOOLS_DIR"
else
    echo "Failed to copy vcpkg tools to $TOOLS_DIR"
fi

cd ./engine-computer
echo "********** MACH: Building the project **********"
cmake --build build --config release
