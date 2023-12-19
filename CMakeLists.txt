cmake_minimum_required(VERSION 3.2)
include("cmake/HunterGate.cmake")

# package manager
HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.25.3.tar.gz"
        SHA1 "0dfbc2cb5c4cf7e83533733bdfd2125ff96680cb"
)

project(engine_computer)

# add package
# add package to cmake
hunter_add_package(Boost COMPONENTS system)
find_package(Boost CONFIG REQUIRED system)

# add executable
add_executable(engine_computer
        main.cpp)

# link libs
if(MINGW)
    target_link_libraries(engine_computer PRIVATE ws2_32)
endif()
target_link_libraries(engine_computer PUBLIC Boost::boost Boost::system)