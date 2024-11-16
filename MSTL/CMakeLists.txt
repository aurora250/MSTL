cmake_minimum_required(VERSION 3.15.0)
project(MSTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR} /)

add_executable(${CMAKE_PROJECT_NAME} main.cpp algo.hpp algobase.cpp algobase.h algorithm.hpp array.hpp basemstlc++.hpp
    basiclib.cpp basiclib.h )
