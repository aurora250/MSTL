cmake_minimum_required(VERSION 3.17)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
endif()

set(MSTL_ENABLE_CUDA OFF CACHE BOOL "Disable CUDA GPU acceleration" FORCE)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSTL_ENABLE_CUDA)
    project(TRY VERSION 1.2.0 LANGUAGES CXX CUDA)

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    set(CMAKE_CUDA_ARCHITECTURES 89) # RTX40
else()
    project(TRY VERSION 1.2.0 LANGUAGES CXX)
endif()


add_subdirectory(MSTL)

add_executable(${CMAKE_PROJECT_NAME}
        "main.cpp" "try.cpp" "try.h")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE MSTL)
