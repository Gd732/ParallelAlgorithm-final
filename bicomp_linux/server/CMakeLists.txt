cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 11)

include_directories(./incl)

set(SOURCES
    src/server.cpp
    src/server_configs.cpp
    src/utils_max_sum.cpp
    src/utils_ms_v1.cpp
)

add_executable(server ${SOURCES})
