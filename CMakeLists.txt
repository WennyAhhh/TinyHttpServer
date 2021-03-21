cmake_minimum_required(VERSION 3.5)

project(buffer_test)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    # test/buffer_test.cc
    src/buffer.cc
    test/log_test.cc
    src/log.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(buffer_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)