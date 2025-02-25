# Macro to add tests
macro(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE PSO)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    message(STATUS "Including test ${name}")
    find_package(OpenMP REQUIRED)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(${name} PRIVATE OpenMP::OpenMP_CXX)
    endif()
endmacro()

# Test executables
add_test_executable(point_test)