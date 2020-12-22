cmake_minimum_required(VERSION 3.5)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS System REQUIRED)
find_package(Threads)

include_directories(${BOOST_INCLUDE_DIRS})

add_executable(server
    ${CMAKE_CURRENT_SOURCE_DIR}/clients.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp)

target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} ${BOOST_LIBRARIES})
