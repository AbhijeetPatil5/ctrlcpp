cmake_minimum_required(VERSION 4.4.4)
project(ctrlcpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(ctrlcpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BUILD_DIR}/include>
    $<INSTALL_INTERFACE:include>)
 
add_executable(hello sandbox/day01/hello.cpp)
add_executable(thermal_euler sandbox/day02/thermal_euler.cpp)