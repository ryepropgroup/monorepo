#!/bin/bash
set VCPKG_BUILD_TYPE=release

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
echo "********** MACH: Configuring the build with CMake **********"
cmake --preset=linux

else
export CC=/opt/homebrew/bin/gcc-14
export CXX=/opt/homebrew/bin/g++-14
cmake --preset=macos
fi

echo "********** MACH: Building the project **********"
cmake --build build #-DCMAKE_BUILD_TYPE=MinSizeRel
cp ./build/engine_computer ../build/bin
case $(uname -s) in
    Linux*)     OSPATH=x64-linux;;
    Darwin*)    OSPATH=arm64-osx;;
    *)          OSPATH=windows
esac

if [ ! -d "../tools" ]; then
  mkdir -p "../tools"
  echo "Directory created: monorepo/tools"
else
  echo "Directory already exists: monorepo/tools"
  rm -rf ../tools
  mkdir -p "../tools"
fi
mv ./build/vcpkg_installed/$OSPATH/tools ../
if [ $? -eq 0 ]; then
  echo "Tools moved to monorepo/tools"
else
  echo "Failed to move tools to monorepo/tools"
fi