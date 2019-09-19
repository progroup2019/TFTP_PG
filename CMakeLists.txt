cmake_minimum_required(VERSION 3.14)
project(TFTP_PG)

set(CMAKE_CXX_STANDARD 14)

add_executable(TFTP_PG main.cpp libs/helpers.cpp libs/helpers.h)