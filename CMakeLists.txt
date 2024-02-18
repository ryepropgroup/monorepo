cmake_minimum_required(VERSION 3.6)
include(FetchContent)
project(engine_computer)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/boost/CMakeLists.txt"
AND NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/spdlog/CMakeLists.txt"
AND NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/yaml-cpp/CMakeLists.txt"
)
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# add package
# add boost package to cmake
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/boost EXCLUDE_FROM_ALL)
include_directories(vendor/labjack)

if (APPLE)
    message("SETTING Apple Library Paths")
    set(LJM "/usr/local/lib/libLabJackM.dylib")
elseif (UNIX)
    message("SETTING Unix/Linux Library Paths!")
    set(LJM "/usr/local/lib/libLabJackM.so")
endif ()

# add executable
add_executable(engine_computer
        src/main.cpp src/labjack.cpp src/config_parser.cpp src/util.cpp src/valve.cpp src/sensor.cpp)


# link libs
if(MINGW)
    target_link_libraries(engine_computer PRIVATE ws2_32)
endif()
target_link_libraries(engine_computer PUBLIC ${LJM} Boost::system Boost::asio spdlog::spdlog yaml-cpp::yaml-cpp)