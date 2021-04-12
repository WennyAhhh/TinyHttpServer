cmake_minimum_required(VERSION 3.5)

project(log_test)

set(CMAKE_CXX_STANDARD 17)

add_library(log_library STATIC
    ${LIB_SOURCE}/base/buffer.cc
    ${LIB_SOURCE}/base/log.cc
)

target_include_directories(log_library
    PUBLIC 
        ${LIB_SOURCE}
)

add_executable(log_test
    log_test.cc
)

target_link_libraries(log_test
    PUBLIC 
        log_library
        pthread
)