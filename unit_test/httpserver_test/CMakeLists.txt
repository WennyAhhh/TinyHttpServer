cmake_minimum_required(VERSION 3.5)

project(http_test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBS_BASE "${LIB_SOURCE}/base/*.cc")
file(GLOB LIBS_EVENT "${LIB_SOURCE}/event/*.cc")
file(GLOB LIBS_SERVER "${LIB_SOURCE}/server/*.cc")
file(GLOB LIBS_HTTP "${LIB_SOURCE}/*.cc")
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
    ${LIBS_SERVER}
    ${LIBS_HTTP}
)
add_executable(http_test
    httprun_test.cc    
    ${SOURCES}
)

target_link_libraries(http_test
    PUBLIC 
        pthread
)

target_include_directories(http_test
    PUBLIC 
        ${LIB_SOURCE}
)