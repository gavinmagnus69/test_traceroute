cmake_minimum_required(VERSION 3.20)

project(traceroute)
set(CMAKE_CXX_STANDARD 17)
find_package(GTest CONFIG REQUIRED)

enable_testing()

add_executable(traceroute ./src/main.cpp)
add_executable(testing ./tests/test.cpp ./src/Traceroute.hpp)

target_link_libraries(testing gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(testing)

