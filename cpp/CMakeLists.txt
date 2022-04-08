cmake_minimum_required(VERSION 3.23)

project(Test)

set(CMAKE_CXX_STANDARD 14)

add_executable(Test main.cpp)
#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

target_link_libraries(Test cppzmq)
