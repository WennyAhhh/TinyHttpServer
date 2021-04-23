cmake_minimum_required(VERSION 3.5)

project(http_test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBS_BASE "${LIB_SOURCE}/base/*.cc")
file(GLOB LIBS_EVENT "${LIB_SOURCE}/event/*.cc")
file(GLOB LIBS_SERVER "${LIB_SOURCE}/server/*.cc")
file(GLOB LIBS_HTTP "${LIB_SOURCE}/http/*.cc")
file(GLOB LIBS_SQL "${LIB_SOURCE}/sql/*.cc")
file(GLOB LIBS_WEB "${LIB_SOURCE}/*.cc")
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
    ${LIBS_WEB}
    ${LIBS_SQL}
)
add_executable(web_test
    web_test.cc
    ${SOURCES}
)

target_link_libraries(web_test
    PUBLIC 
        pthread
        mysqlclient
)

target_include_directories(web_test
    PUBLIC 
        ${LIB_SOURCE}
)