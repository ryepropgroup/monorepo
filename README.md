# MACH Engine Computer

## Build Instructions - Windows
1. Download the [software and drivers for LabJack here](https://support.labjack.com/docs/ljm-software-installer-downloads-t4-t7-t8-digit). The only required component is [LabJackM](https://support.labjack.com/docs/ljm-library-overview), the driver and library for communication with LabJack devices.
2. The [vcpkg package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) is included as a submodule in this project. Please run `bootstrap.bat` to download the submodule and set it up.
3. This project uses the Microsoft Visual C++ Compiler (MSVC). Please download `Build Tools for Visual Studio` [from the official website](https://visualstudio.microsoft.com/downloads/) or [use the direct link here](https://aka.ms/vs/17/release/vs_BuildTools.exe). Alternatively, you can download the full Visual Studio suite on the same page.
4. Run `build.bat` in a command prompt or terminal of your choice to build the executable `build/engine_computer.exe`.
5. Change directory to the build folder with `cd build` and run the executable with `engine_computer` or `./engine_computer` (on git bash).

## Build Instructions - Linux
1. Download the [software and drivers for LabJack here](https://support.labjack.com/docs/ljm-software-installer-downloads-t4-t7-t8-digit). The only required component is [LabJackM](https://support.labjack.com/docs/ljm-library-overview), the driver and library for communication with LabJack devices.
2. The [vcpkg package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) is included as a submodule in this project. Please run `bootstrap.sh` to set it up. It may prompt you to install some dependencies (`curl`, `zip`, `unzip`, `tar`) if you don't have them already.
3. Run `build.sh` in a command prompt or terminal of your choice to build the executable `build/engine_computer`.
4. Change directory to the build folder with `cd build` and run the executable with `./engine_computer`.