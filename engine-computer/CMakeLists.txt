cmake_minimum_required(VERSION 3.15)
project(engine_computer)
set (CMAKE_CXX_STANDARD 20)

find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core)

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
    set(LJM_FOLDER "C:/Program Files (x86)/LabJack/Drivers/64bit")
    set(LJM "${LJM_FOLDER}/LabJackM.lib")
endif () 

if (NOT EXISTS ${LJM})
    message(FATAL_ERROR "FATAL: LJM installation not found!")
endif()

include_directories(${LJM_FOLDER})
include_directories(vendor/labjack)
include_directories(src)
include_directories(include)
include_directories(generated)

# add executable
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/generated/*.cc" "${PROJECT_SOURCE_DIR}/vendor/labjack/*.cpp")
else()
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/generated/*.cc" "${PROJECT_SOURCE_DIR}/vendor/labjack/*.cpp")
endif()
add_executable(engine_computer ${SOURCES})

# Copy resources folder
file (COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# link libs
if(MINGW) 
    target_link_libraries(engine_computer PRIVATE ws2_32) 
endif()
target_link_libraries(engine_computer PRIVATE ${LJM} spdlog::spdlog yaml-cpp::yaml-cpp
    absl::check absl::flags absl::flags_parse absl::log 
    gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf
    wx::core)