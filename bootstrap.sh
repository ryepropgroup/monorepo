#!/bin/bash
echo "********** MACH: Updating git submodules **********"
git submodule update --init --recursive

cd vendor/vcpkg
if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    echo "********** MACH: Bootstrapping vcpkg (windows) **********"
    ./bootstrap-vcpkg.bat
else
    echo "********** MACH: Bootstrapping vcpkg (unix) **********"
    ./bootstrap-vcpkg.sh
fi