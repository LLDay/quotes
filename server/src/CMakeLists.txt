cmake_minimum_required(VERSION 3.5)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system serialization program_options REQUIRED)
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(
    PROTO_SRCS
    PROTO_HDRS
    ${CMAKE_SOURCE_DIR}/quotes.proto
)

add_executable(server
    ${CMAKE_CURRENT_SOURCE_DIR}/clients_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/asset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/assets_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/session.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)

target_link_libraries(server
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${Protobuf_LIBRARIES}
)
