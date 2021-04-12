cmake_minimum_required(VERSION 3.5)

project(time_test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBS_BASE "${LIB_SOURCE}/base/*.cc")
file(GLOB LIBS_EVENT "${LIB_SOURCE}/event/*.cc")

add_library(time_library STATIC
    ${LIBS_BASE}
    ${LIBS_EVENT}
)

target_include_directories(time_library
    PUBLIC 
        ${LIB_SOURCE}
)

add_executable(time_test
    time_test.cc
)

target_link_libraries(time_test
    PUBLIC 
        time_library
        pthread
)