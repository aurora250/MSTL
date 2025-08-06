cmake_minimum_required(VERSION 3.17)

if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/WX)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "BUILD TYPE：Debug (MSVC)")
        add_compile_options(/Od /Zi)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "BUILD TYPE：Release (MSVC)")
        add_compile_options(/O2)
    endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--emit-relocs")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Werror)
        message(STATUS "BUILD TYPE：Debug (GNU/Clang)")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "BUILD TYPE：Release (GNU/Clang)")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()

else()
    message(WARNING "Unsupported Compiler: UTF-8 Encoding Options Not Set")
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Current Arch: x64")
else()
    message(STATUS "Current Arch: x86")
endif()


# ------------------- Boolean Global Settings -------------------

# Use FORCE setting to overlaid these booleans
if(NOT DEFINED MSTL_ENABLE_BOOST)
    set(MSTL_ENABLE_BOOST ON CACHE BOOL "Enable Boost library support")
endif()
if(NOT DEFINED MSTL_ENABLE_MYSQL)
    set(MSTL_ENABLE_MYSQL ON CACHE BOOL "Enable MySQL database support")
endif()
if(NOT DEFINED MSTL_ENABLE_CUDA)
    set(MSTL_ENABLE_CUDA ON CACHE BOOL "Enable CUDA GPU acceleration")
endif()

message(STATUS "Project Configuration:")
message(STATUS "  MSTL_ENABLE_BOOST: ${MSTL_ENABLE_BOOST}")
message(STATUS "  MSTL_ENABLE_MYSQL: ${MSTL_ENABLE_MYSQL}")
message(STATUS "  MSTL_ENABLE_CUDA : ${MSTL_ENABLE_CUDA}")


# ------------------- File Selection -------------------
file(GLOB_RECURSE SUB_DIR_SRC_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(SUB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "MSTL project codes")
include_directories(${SUB_INCLUDE_DIR})

add_library(MSTL STATIC ${SUB_DIR_SRC_LIST}
        datetime.cpp)

set_target_properties(MSTL PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(MSTL PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


# ------------------- Boost -------------------
if(MSTL_ENABLE_BOOST AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(POLICY CMP0167)
        cmake_policy(SET CMP0167 OLD)
    endif()

    find_package(Boost QUIET COMPONENTS system filesystem)

    if(Boost_FOUND)
        target_link_libraries(MSTL PUBLIC Boost::system Boost::filesystem)
        target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_BOOST__)
    else()
        message(WARNING "Boost find failed.")
        message(WARNING "Boost link failed.")
        message(WARNING "Your project can not use Boost components.")
    endif()
else()
    message(STATUS "Boost support disabled.")
endif()


# ------------------- MySQL -------------------
if(MSTL_ENABLE_MYSQL AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(WIN32)
        set(MYSQL_ROOT_DIR "D:/MySQL/MySQL Server 8.0" CACHE PATH "MySQL root directory for Windows")
        find_path(MySQL_INCLUDE_DIRS NAMES mysql.h HINTS "${MYSQL_ROOT_DIR}/include" NO_DEFAULT_PATH QUIET)
        find_library(MySQL_LIBRARIES NAMES libmysql.lib HINTS "${MYSQL_ROOT_DIR}/lib" NO_DEFAULT_PATH QUIET)
    elseif(UNIX)
        find_path(MySQL_INCLUDE_DIRS NAMES mysql.h HINTS "/usr/include/mysql" NO_DEFAULT_PATH QUIET)
        find_library(MySQL_LIBRARIES NAMES libmysqlclient.so HINTS "/usr/lib/x86_64-linux-gnu" NO_DEFAULT_PATH QUIET)
    else()
        message(WARNING "MySQL find failed.")
    endif()

    if(MySQL_INCLUDE_DIRS AND MySQL_LIBRARIES)
        target_include_directories(MSTL PUBLIC ${MySQL_INCLUDE_DIRS})
        target_link_libraries(MSTL PUBLIC ${MySQL_LIBRARIES})
        target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_MYSQL__)
    else()
        message(WARNING "MySQL link failed.")
        message(WARNING "your project can not use MySQL components.")
    endif()
else()
    message(STATUS "MySQL support disabled.")
endif()


# ------------------- CUDA -------------------
if(MSTL_ENABLE_CUDA AND MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(DEFINED CMAKE_CUDA_COMPILER)
        find_package(CUDAToolkit QUIET REQUIRED)

        if(CUDAToolkit_FOUND)
            set_target_properties(MSTL PROPERTIES
                    CUDA_SEPARABLE_COMPILATION ON
                    CUDA_RESOLVE_DEVICE_SYMBOLS ON
            )
            target_link_libraries(MSTL PRIVATE CUDA::cudart)
            # This command can make constexpr functions can run in GPU, but those funcs can`t use functions like printf
            # target_compile_options(MSTL PUBLIC $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)
            target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_CUDA__)
            set(CUDA_CONFIG_STATE ON CACHE BOOL "CUDA find and link succeed.")
        else()
            message(WARNING "CUDA link failed.")
            set(CUDA_CONFIG_STATE OFF CACHE BOOL "CUDA link failed.")
        endif()
    else()
        message(WARNING "CUDA find failed.")
        set(CUDA_CONFIG_STATE OFF CACHE BOOL "CUDA find failed.")
    endif()

    if(NOT CUDA_CONFIG_STATE)
        message(WARNING "your project can not use CUDA components.")
    endif()
else()
    message(STATUS "CUDA support disabled.")
endif()
