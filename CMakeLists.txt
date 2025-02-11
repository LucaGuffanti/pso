cmake_policy(SET CMP0048 NEW)
project(PSO VERSION 0.1)

cmake_minimum_required(VERSION 3.10)

set(LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

# ====== Really Basic CMake configuration for now ======
add_executable(point_test test/point_test.cpp src/Point.cpp)
target_include_directories(point_test PRIVATE include)