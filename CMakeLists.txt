cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0104 NEW)
cmake_policy(SET CMP0074 NEW)

project(PSO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED True)

# Check if CUDA is available
find_package(CUDA QUIET)


if (CUDA_FOUND)
    enable_language(CUDA)
    set(PSO_CUDA_PRESENT On)
    message(STATUS "CUDA is available. PSO_CUDA_PRESENT set to On.")
else()
    set(PSO_CUDA_PRESENT Off)
    message(STATUS "CUDA is not available. PSO_CUDA_PRESENT set to Off.")
endif()

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


if (PSO_CUDA_PRESENT)
    file(GLOB_RECURSE PSO_CUDA_SOURCES src/gpu/*.cu)
    list(APPEND PSO_SOURCES ${PSO_CUDA_SOURCES})
endif()


add_library(PSO ${PSO_SOURCES})

if (PSO_CUDA_PRESENT)
    set_target_properties(PSO PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        POSITION_INDEPENDENT_CODE ON
        CMAKE_CUDA_ARCHITECTURES "50;60;70;80"
        )

    target_link_libraries(PSO PUBLIC ${CUDA_LIBRARIES})
endif()

# ====== Set the include directories ======
set(PSO_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/util
    ${CMAKE_CURRENT_SOURCE_DIR}/include/output
    ${CMAKE_CURRENT_SOURCE_DIR}/include/functions
)

if (PSO_CUDA_PRESENT)
    list(APPEND PSO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/gpu)
endif()

target_include_directories(PSO PUBLIC 
    ${PSO_INCLUDE_DIRS}
)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

# Link MPI to the PSO library
if (MPI_CXX_FOUND)
    target_link_libraries(PSO PUBLIC MPI::MPI_CXX)
endif()

# Link OpenMP to the PSO library
if (OpenMP_CXX_FOUND)
    target_link_libraries(PSO PUBLIC OpenMP::OpenMP_CXX)
endif()


add_subdirectory(test)
