cmake_minimum_required(VERSION 3.10)

project(csv)
include_directories(../csv)

set(SOURCE_LIB Server.cpp)
add_library(server ${SOURCE_LIB})

target_link_libraries(server csv) 