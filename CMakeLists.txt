cmake_minimum_required(VERSION 3.6)
include(FetchContent)
project(engine_computer)

find_package(boost CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)


if (APPLE)
    message("SETTING Apple Library Paths")
    set(LJM "/usr/local/lib/libLabJackM.dylib")
elseif (UNIX)
    message("SETTING Unix/Linux Library Paths!")
    set(LJM "/usr/local/lib/libLabJackM.so")
elseif (WIN32)
    message("SETTING Windows Library Paths!")
    set(LJM "${PROJECT_SOURCE_DIR}/vendor/labjack/LabJackM.lib")
endif ()

# add executable
add_executable(engine_computer
        src/main.cpp src/labjack.cpp 
        src/config_parser.cpp src/util.cpp 
        src/valve.cpp src/sensor.cpp)

# link libs
if(MINGW)
    target_link_libraries(engine_computer PRIVATE ws2_32)
endif()
target_link_libraries(engine_computer PUBLIC ${LJM} Boost::system Boost::asio spdlog::spdlog yaml-cpp::yaml-cpp)