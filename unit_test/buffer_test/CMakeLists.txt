cmake_minimum_required(VERSION 3.5)

project(buffer_test)

set(CMAKE_CXX_STANDARD 17)

# set(LIBRARY_SOURCE ../../src)

# MESSAGE( STATUS "this var key = ${LIBRARY_SOURCE}.")

add_library(buffer STATIC
    PUBLIC
        ../../src/base/buffer.cc
)

target_include_directories(buffer
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/src
)

add_executable(buffer_test
    PUBLIC 
        buffer_test.cc
)

target_link_libraries(buffer_test
    PUBLIC 
        buffer
)