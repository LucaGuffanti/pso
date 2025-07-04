# Macro to add tests
macro(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE PSO)
    target_include_directories(${name} PRIVATE 
        ${PSO_INCLUDE_DIRS}
    )
    message(STATUS "Including test ${name}")
endmacro()

# Test executables
add_test_executable(point_test)
add_test_executable(function_test)
add_test_executable(txt_writer_test)




if (${PSO_CUDA_PRESENT})
    add_test_executable(test_gpu)
    # add_test_executable(culo)

endif()