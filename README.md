# MACH Engine Computer

## Setup 
1. Download the [software and drivers for LabJack here](https://support.labjack.com/docs/ljm-software-installer-downloads-t4-t7-t8-digit). The only required software is [LabJackM](https://support.labjack.com/docs/ljm-library-overview), the driver and library for communication with LabJack devices.
2. Install the [vcpkg package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started). This isn't requried on Windows if you use (or are going to install) Microsoft Visual Studio, since it comes bundled.

## Windows Build/Run Instructions
1. This project uses the Microsoft Visual C++ Compiler (MSVC). Please find and download `Build Tools for Visual Studio` [from the official website](https://visualstudio.microsoft.com/downloads/) or [use the direct link here](https://aka.ms/vs/17/release/vs_BuildTools.exe). Alternatively, you can download the full Visual Studio suite on the same page.
2. Run `winbuild.bat` in a command prompt or terminal of your choice to build the executable `build/engine_computer.exe`.
3. Change directory to build with `cd build` and run the executable by typing `engine_computer` or `./engine_computer` (on bash).
