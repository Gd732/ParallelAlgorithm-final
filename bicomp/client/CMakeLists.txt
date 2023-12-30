cmake_minimum_required(VERSION 3.10)

project(MyClient)

set(CMAKE_CXX_STANDARD 11)

include_directories(./incl)

set(SOURCES
    src/client.cpp
    src/client_configs.cpp
    src/utils_ms_v1.cpp
)

add_executable(client ${SOURCES})