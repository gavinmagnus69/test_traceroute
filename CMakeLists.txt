cmake_minimum_required(VERSION 3.20)

project(traceroute)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(GTest CONFIG REQUIRED)

enable_testing()

add_executable(traceroute ./src/main.cpp)
add_executable(testing ./tests/test.cpp ./src/Traceroute.hpp)

target_link_libraries(testing gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(testing)

