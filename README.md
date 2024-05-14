# MACH Engine Computer

## Configuration

`config/config.yml`
```yaml
sensors:
    - name: "name"
      port: "AIN#"
      settings:
        NAME: VALUE # see settings below
```

### Sensor Settings
`RANGE: #.#`: The max voltage (float value) of the sensor plugged into this port (ex. 10.0). Corresponds to the [AIN#_RANGE setting](https://support.labjack.com/docs/14-0-analog-inputs-t-series-datasheet#id-14.0AnalogInputs[T-SeriesDatasheet]-Range/Gain-T7/T8) on LabJack.

`NEGATIVE_PORT: #`: If this sensor is making a differential reading (using a second AIN as a reference point), this is the port number corresponding of the negative channel (ex. 1 for AIN1). Corresponds to the [AIN#_NEGATIVE_CH setting](https://support.labjack.com/docs/14-0-analog-inputs-t-series-datasheet#id-14.0AnalogInputs[T-SeriesDatasheet]-Single-endedorDifferential-T7Only) on LabJack.

`THERMOCOUPLE_TYPE: E, J, K, R, T, S, N, B, or C`: Sets up this sensor as a type X thermocouple, using LabJack's extended features. Defaults to using the on-board CJC sensor, in celsius. Corresponds to the [AIN#_EF_INDEX/AIN#_EF_CONFIG_X settings](https://support.labjack.com/docs/14-1-1-thermocouple-t7-t8-t-series-datasheet) on LabJack.

## Build Instructions - Windows
1. Download the [software and drivers for LabJack here](https://support.labjack.com/docs/ljm-software-installer-downloads-t4-t7-t8-digit). The only required component is [LabJackM](https://support.labjack.com/docs/ljm-library-overview), the driver and library for communication with LabJack devices.
1. The [vcpkg package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) is included as a submodule in this project. Run `bootstrap.bat` to download the submodule and set it up.
1. This project uses the Microsoft Visual C++ Compiler (MSVC) and its bundled build tools ([CMake](https://cmake.org/), and [Ninja](https://ninja-build.org/)). Please download `Build Tools for Visual Studio` [from the official website](https://visualstudio.microsoft.com/downloads/) or [direct link](https://aka.ms/vs/17/release/vs_BuildTools.exe). Alternatively, you can download the full Visual Studio program on the same page.
1. Run `build.bat` in a command prompt or terminal of your choice to build the executable `build/engine_computer.exe`.
1. Change directory to the build folder with `cd build` and run the executable with `engine_computer` (on command prompt) or `./engine_computer` (on git bash).

## Build Instructions - Linux
1. Download the [software and drivers for LabJack here](https://support.labjack.com/docs/ljm-software-installer-downloads-t4-t7-t8-digit) or [direct download (Linux x64)](https://files.labjack.com/installers/LJM/Linux/x64/release/labjack_ljm_software_2019_07_16_x86_64.tar.gz). The only required component is [LabJackM](https://support.labjack.com/docs/ljm-library-overview), the driver and library for communication with LabJack devices.
1. The [vcpkg package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) is included as a submodule in this project. Run `./bootstrap.sh` to set it up. It may prompt you to install some dependencies (`curl`, `zip`, `unzip`, and `tar`) if you don't have them already.
1. Install CMake with `sudo apt install cmake` (on Ubuntu).
1. Install the C++ compiler, g++, with `sudo apt install build-essential`.
1. Install Ninja with `sudo apt install ninja`.
1. Install pkg-config, a dependency of one of our libraries, with `sudo apt install pkg-config`.
1. Run `./build.sh` in a terminal of your choice to build the executable `build/engine_computer`.
1. Change directory to the build folder with `cd build` and run the executable with `./engine_computer`.