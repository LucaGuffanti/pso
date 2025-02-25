cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0048 NEW)
project(PSO VERSION 0.1)

set(LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# ====== Set the build type ======
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ====== Set the compiler flags ======
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# ====== Find the OpenMP package ======
find_package(OpenMP REQUIRED)
# ====== Find the MPI package ======
find_package(MPI REQUIRED)

# ====== Now build the library ======
file(GLOB_RECURSE PSO_SOURCES src/*.cpp)

add_library(PSO ${PSO_SOURCES})
target_include_directories(PSO PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/output
    ${CMAKE_CURRENT_SOURCE_DIR}/include/functions
    ${CMAKE_CURRENT_SOURCE_DIR}/include/util
    # Add more subdirectories as needed
)

add_subdirectory(test)
