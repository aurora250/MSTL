cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
endif()

set(CMAKE_CXX_STANDARD 20)
project(TRY VERSION 1.2.0 LANGUAGES CXX)

add_subdirectory(MSTL)
add_executable(${CMAKE_PROJECT_NAME} "main.cpp" "try.cpp" "try.h")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE MSTL)
