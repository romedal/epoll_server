#How to use
#
# mkdir build - should be sibling of root in case of Eclipse CDT creating
#
# cmake ../tcp_server -G "Eclipse CDT4 - Unix Makefiles"
#

cmake_minimum_required(VERSION 3.10)

project(tcp_server)

set(SOURCE_EXE tcp_server.cpp)

include_directories(csv)

include_directories(server)


add_executable(tcp_server ${SOURCE_EXE})

set_property(TARGET tcp_server PROPERTY COMPILE_OPTIONS "-pthread") 
add_subdirectory(csv)
add_subdirectory(server)

target_link_libraries(tcp_server csv server pthread)