@echo off

for /f "usebackq tokens=*" %%i in (`..\vendor\vswhere\vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if not defined DevEnvDir (
    if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
        echo ********** MACH: Using Visual Studio at %InstallDir% **********
        "%InstallDir%\Common7\Tools\vsdevcmd.bat" %* & "%~nx0" %*
    ) else (
        echo ********** MACH: Visual Studio not found, exiting! **********
        exit /b 1
    )
) else (
    echo ********** MACH: Already using Visual Studio at %InstallDir% **********
)

echo ********** MACH: Configuring the build with CMake **********
cmake --preset=windows

echo ********** MACH: Building the project **********
cmake --build build