cmake_minimum_required(VERSION 3.14)
project(TFTP_PG)

set(CMAKE_CXX_STANDARD 14)

add_library(libs STATIC libs/checksum.h libs/checksum.cpp libs/header.cpp libs/header.h libs/helpers.cpp libs/helpers.h libs/server.cpp libs/server.h libs/client.cpp libs/client.h)

add_executable(TFTP_PG main.cpp)


target_link_libraries(TFTP_PG libs)
