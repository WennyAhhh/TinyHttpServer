cmake_minimum_required(VERSION 3.5)

project(buffer_test)

set(CMAKE_CXX_STANDARD  17)

MESSAGE( STATUS "this var key = ${LIB_SOURCE}.")

add_library(buffer_library STATIC
    ${LIB_SOURCE}/base/buffer.cc
)

target_include_directories(buffer_library
    PUBLIC 
        ${LIB_SOURCE}
)

add_executable(buffer_test
    buffer_test.cc
)

target_link_libraries(buffer_test
    PUBLIC 
        buffer_library
)