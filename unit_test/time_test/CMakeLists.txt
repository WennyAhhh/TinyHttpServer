cmake_minimum_required(VERSION 3.5)

project(time_test)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "${ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBS_BASE "${LIB_SOURCE}/base/*.cc")
file(GLOB LIBS_EVENT "${LIB_SOURCE}/event/*.cc")

# add_library(time_library STATIC
#     ${LIBS_BASE}
#     ${LIBS_EVENT}
# )

# target_include_directories(time_library
#     PUBLIC 
#         ${LIB_SOURCE}
# )
set(SOURCES
    ${LIBS_BASE}
    ${LIBS_EVENT}
)
add_executable(time_test
    time_test.cc
    ${SOURCES}
)

target_link_libraries(time_test
    PUBLIC 
        pthread
)

target_include_directories(time_test
    PUBLIC 
        ${LIB_SOURCE}
)