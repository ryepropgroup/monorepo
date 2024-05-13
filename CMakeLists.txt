cmake_minimum_required(VERSION 3.6)
include(FetchContent)
project(engine_computer)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS exception)
find_package(Boost REQUIRED COMPONENTS asio)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

if (APPLE)
    message("CMake: SETTING Apple Library Paths")
    set(LJM_FOLDER "/usr/local/lib")
    set(LJM "${LJM_FOLDER}/libLabJackM.dylib")
elseif (UNIX)
    message("CMake: SETTING Unix/Linux Library Paths!")
    set(LJM_FOLDER "/usr/local/lib")
    set(LJM "${LJM_FOLDER}/libLabJackM.so")
elseif (WIN32)
    message("CMake: SETTING Windows Library Paths!")
    set(LJM_FOLDER "C:/Program Files (x86)/LabJack/Drivers")
    set(LJM "${LJM_FOLDER}/LabJackM.lib")
endif () 

if (NOT EXISTS ${LJM})
    message(FATAL_ERROR "FATAL: LJM installation not found!")
endif()

include_directories(${LJM_FOLDER})

# add executable
add_executable(engine_computer
        src/main.cpp src/labjack.cpp 
        src/config_parser.cpp src/util.cpp 
        src/valve.cpp src/sensor.cpp)

# link libs
if(MINGW) 
    target_link_libraries(engine_computer PRIVATE ws2_32) 
endif()
target_link_libraries(engine_computer PUBLIC ${LJM} Boost::asio Boost::exception spdlog::spdlog yaml-cpp::yaml-cpp)