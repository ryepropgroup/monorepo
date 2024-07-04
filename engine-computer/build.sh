
#!/bin/bash

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    cmd.exe /c .buildvs.bat
    exit 1

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "********** MACH: Configuring the build with CMake **********"
    cmake --preset=linux

else
    echo "********** MACH: Configuring the build with CMake **********"
    export CC=/opt/homebrew/bin/gcc-14
    export CXX=/opt/homebrew/bin/g++-14
    cmake --preset=macos
fi

echo "********** MACH: Building the project **********"
cmake --build build 
cp ./build/engine_computer ../build/bin