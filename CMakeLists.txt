cmake_minimum_required(VERSION 3.15)
project(cpp_chess)

set(CMAKE_CXX_STANDARD 14)

add_library(cpp_chess chess.cpp)

add_executable(test tests.cpp)