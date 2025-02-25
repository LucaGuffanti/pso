cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0048 NEW)
project(PSO VERSION 0.1)

set(LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ====== Find the OpenMP package ======
find_package(OpenMP REQUIRED)
# ====== Find the MPI package ======
find_package(MPI REQUIRED)

# ====== Now build the library ======
file(GLOB_RECURSE PSO_SOURCES src/*.cpp)

add_library(PSO ${PSO_SOURCES})
target_include_directories(PSO PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(test)
