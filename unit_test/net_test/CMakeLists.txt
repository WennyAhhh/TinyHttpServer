cmake_minimum_required(VERSION 3.5)

project(server_test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBS_BASE "${LIB_SOURCE}/base/*.cc")
file(GLOB LIBS_EVENT "${LIB_SOURCE}/event/*.cc")
file(GLOB LIBS_SERVER "${LIB_SOURCE}/server/*.cc")

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
)
add_executable(net_test
    # inetaddress.cc
    # tcpserver_send.cc
    tcpserver.cc
    ${SOURCES}
)

target_link_libraries(net_test
    PUBLIC 
        pthread
)

target_include_directories(net_test
    PUBLIC 
        ${LIB_SOURCE}
)