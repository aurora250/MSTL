cmake_minimum_required(VERSION 3.17)
project(MSTL LANGUAGES CXX)

# ------------------- 设置默认构建类型 -------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ------------------- C++标准设置 -------------------
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard used by MSTL (default: 17)")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_STANDARD LESS 14)
    message(FATAL_ERROR "MSTL can only be compiled with C++14 or higher standard")
endif()

# ------------------- 编译器配置函数 -------------------
function(configure_compiler_options target)
    # 通用UTF-8支持
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
        # 在Debug模式下启用警告即错误
        target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:/WX /Od /Zi>
                $<$<CONFIG:Release>:/O2>
        )
        message(STATUS "Configured MSVC compiler options for ${CMAKE_BUILD_TYPE}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
                -finput-charset=UTF-8 -fexec-charset=UTF-8
                $<$<CONFIG:Debug>:-Werror -O0 -g>
                $<$<CONFIG:Release>:-O2>
        )
        # 链接器选项
        target_link_options(${target} PRIVATE -Wl,--emit-relocs)
        message(STATUS "Configured GNU/Clang compiler options for ${CMAKE_BUILD_TYPE}")
    else()
        message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# ------------------- 架构检测 -------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MSTL_ARCH "x64")
    message(STATUS "Current Architecture: x64")
else()
    set(MSTL_ARCH "x86")
    message(STATUS "Current Architecture: x86")
endif()

message(STATUS "C++ Standard Version: ${CMAKE_CXX_STANDARD}")

# ------------------- 配置选项 -------------------
option(MSTL_ENABLE_BOOST "Enable Boost library support" ON)
option(MSTL_ENABLE_MYSQL "Enable MySQL database support" ON)
option(MSTL_ENABLE_SQLITE3 "Enable SQLite3 database support" ON)
option(MSTL_ENABLE_REDIS "Enable Redis database support" ON)
option(MSTL_ENABLE_QT6 "Enable Qt6 toolchain support" ON)
option(MSTL_ENABLE_CUDA "Enable CUDA GPU acceleration" OFF)

message(STATUS "Project Configuration:")
message(STATUS "  MSTL_ENABLE_BOOST:   ${MSTL_ENABLE_BOOST}")
message(STATUS "  MSTL_ENABLE_MYSQL:   ${MSTL_ENABLE_MYSQL}")
message(STATUS "  MSTL_ENABLE_SQLITE3: ${MSTL_ENABLE_SQLITE3}")
message(STATUS "  MSTL_ENABLE_REDIS:   ${MSTL_ENABLE_REDIS}")
message(STATUS "  MSTL_ENABLE_QT6:     ${MSTL_ENABLE_QT6}")
message(STATUS "  MSTL_ENABLE_CUDA:    ${MSTL_ENABLE_CUDA}")

# ------------------- 源文件定义 -------------------
set(ROOT_SRC_FILES
        algo.hpp algobase.hpp algorithm.hpp any.hpp array.hpp
        basic_string.hpp basiclib.hpp bitmap.hpp check_type.hpp
        concepts.hpp database_pool.hpp datetime.hpp deque.hpp
        errorlib.hpp file.hpp functional.hpp functor.hpp
        hashtable.hpp heap.hpp hexadecimal.hpp iterator.hpp
        json.hpp leonardo_heap.hpp list.hpp lock_free_queue.hpp
        map.hpp mathlib.hpp memory.hpp mstlc++.hpp numeric.hpp
        optional.hpp print.hpp queue.hpp random.hpp rb_tree.hpp
        set.hpp sort.hpp stack.hpp string.hpp string_view.hpp
        stringstream.hpp thread_pool.hpp timer.hpp trace_memory.hpp
        tuple.hpp type_traits.hpp undef_cmacro.hpp unordered_map.hpp
        unordered_set.hpp utility.hpp variant.hpp vector.hpp
)

set(WEB_SRC_FILES
        web/dns.hpp web/session.cpp web/session.hpp
        web/servlet.cpp web/servlet.hpp web/socket.hpp
)

# ------------------- 创建目标 -------------------
add_library(MSTL SHARED ${ROOT_SRC_FILES} ${WEB_SRC_FILES})

# 设置目标属性
set_target_properties(MSTL PROPERTIES
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        LINKER_LANGUAGE CXX
        VERSION 1.3.0
        SOVERSION 1
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows特定设置
if(WIN32)
    set_target_properties(MSTL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_link_libraries(MSTL PUBLIC ws2_32 wsock32)
endif()

target_include_directories(MSTL PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(MSTL PRIVATE MSTL_EXPORTS)

# 配置编译器选项
configure_compiler_options(MSTL)

# ------------------- 第三方库查找辅助函数 -------------------
function(find_and_link_library target lib_name)
    set(options REQUIRED)
    set(oneValueArgs CONDITION)
    set(multiValueArgs COMPONENTS DEFINITIONS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(DEFINED ARGS_CONDITION AND NOT ${ARGS_CONDITION})
        message(STATUS "${lib_name} support disabled by condition")
        return()
    endif()

    find_package(${lib_name} ${ARGS_COMPONENTS} QUIET)

    if(${lib_name}_FOUND)
        foreach(component IN LISTS ARGS_COMPONENTS)
            if(component STREQUAL "")
                target_link_libraries(${target} PUBLIC ${lib_name}::${lib_name})
            else()
                target_link_libraries(${target} PUBLIC ${lib_name}::${component})
            endif()
        endforeach()

        foreach(definition IN LISTS ARGS_DEFINITIONS)
            target_compile_definitions(${target} PUBLIC ${definition})
        endforeach()

        message(STATUS "${lib_name} found and linked successfully")
    else()
        if(ARGS_REQUIRED)
            message(FATAL_ERROR "${lib_name} is required but not found")
        else()
            message(WARNING "${lib_name} not found - related functionality will be disabled")
        endif()
    endif()
endfunction()

# ------------------- Boost配置 -------------------
if(MSTL_ENABLE_BOOST AND MSTL_ARCH STREQUAL "x64")
    if(POLICY CMP0167)
        cmake_policy(SET CMP0167 OLD)
    endif()

    find_package(Boost QUIET COMPONENTS system filesystem)
    if(Boost_FOUND)
        target_link_libraries(MSTL PUBLIC Boost::system Boost::filesystem)
        target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_BOOST__)
        message(STATUS "Boost linked successfully")
    else()
        message(WARNING "Boost not found - Boost components will be unavailable")
    endif()
else()
    message(STATUS "Boost support disabled")
endif()

# ------------------- 数据库支持检查函数 -------------------
function(enable_database_support target)
    get_target_property(existing_defs ${target} COMPILE_DEFINITIONS)
    if(existing_defs)
        list(FIND existing_defs "MSTL_SUPPORT_DB__" db_support_index)
        if(db_support_index EQUAL -1)
            target_compile_definitions(${target} PUBLIC MSTL_SUPPORT_DB__)
        endif()
    else()
        target_compile_definitions(${target} PUBLIC MSTL_SUPPORT_DB__)
    endif()
endfunction()

# ------------------- MySQL配置 -------------------
if(MSTL_ENABLE_MYSQL AND MSTL_ARCH STREQUAL "x64")
    set(MySQL_FOUND FALSE)

    if(WIN32)
        set(MYSQL_ROOT_DIR "D:/MySQL/MySQL Server 8.0" CACHE PATH "MySQL root directory")
        find_path(MySQL_INCLUDE_DIR NAMES mysql.h
                HINTS "${MYSQL_ROOT_DIR}/include"
                PATHS ENV MYSQL_DIR PATH_SUFFIXES include
                NO_DEFAULT_PATH)
        find_library(MySQL_LIBRARY NAMES libmysql mysqlclient
                HINTS "${MYSQL_ROOT_DIR}/lib"
                PATHS ENV MYSQL_DIR PATH_SUFFIXES lib
                NO_DEFAULT_PATH)
    else()
        find_path(MySQL_INCLUDE_DIR NAMES mysql.h mysql/mysql.h
                PATHS /usr/include/mysql /usr/local/include/mysql /opt/mysql/include)
        find_library(MySQL_LIBRARY NAMES mysqlclient
                PATHS /usr/lib /usr/local/lib /opt/mysql/lib
                PATH_SUFFIXES x86_64-linux-gnu)
    endif()

    if(MySQL_INCLUDE_DIR AND MySQL_LIBRARY)
        set(MySQL_FOUND TRUE)
        target_include_directories(MSTL PUBLIC ${MySQL_INCLUDE_DIR})
        target_link_libraries(MSTL PUBLIC ${MySQL_LIBRARY})
        target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_MYSQL__)
        enable_database_support(MSTL)
        message(STATUS "MySQL linked successfully")
    else()
        message(WARNING "MySQL not found - MySQL components will be unavailable")
    endif()
else()
    message(STATUS "MySQL support disabled")
endif()

# ------------------- SQLite3配置 -------------------
if(MSTL_ENABLE_SQLITE3 AND MSTL_ARCH STREQUAL "x64")
    if(WIN32)
        set(SQLITE3_ROOT_DIR "D:/sqlite3" CACHE PATH "SQLite3 root directory")
        find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
                HINTS "${SQLITE3_ROOT_DIR}/include"
                NO_DEFAULT_PATH)
        find_library(SQLITE3_LIBRARY NAMES sqlite3
                HINTS "${SQLITE3_ROOT_DIR}/lib"
                NO_DEFAULT_PATH)

        if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
            target_include_directories(MSTL PUBLIC ${SQLITE3_INCLUDE_DIR})
            target_link_libraries(MSTL PUBLIC ${SQLITE3_LIBRARY})
            target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_SQLITE3__)
            enable_database_support(MSTL)
            message(STATUS "SQLite3 linked successfully")
        else()
            message(WARNING "SQLite3 not found - SQLite3 components will be unavailable")
        endif()
    else()
        find_package(SQLite3 QUIET)
        if(SQLite3_FOUND)
            target_link_libraries(MSTL PUBLIC SQLite::SQLite3)
            target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_SQLITE3__)
            enable_database_support(MSTL)
            message(STATUS "SQLite3 linked successfully")
        else()
            message(WARNING "SQLite3 not found - SQLite3 components will be unavailable")
        endif()
    endif()
else()
    message(STATUS "SQLite3 support disabled")
endif()

# ------------------- Redis配置 -------------------
if(MSTL_ENABLE_REDIS AND MSTL_ARCH STREQUAL "x64")
    set(HIREDIS_FOUND FALSE)

    if(WIN32)
        set(HIREDIS_ROOT_DIR "D:/vcpkg/packages/hiredis_x64-windows" CACHE PATH "Hiredis root directory")
        find_path(HIREDIS_INCLUDE_DIR NAMES hiredis.h
                HINTS "${HIREDIS_ROOT_DIR}/include/hiredis"
                NO_DEFAULT_PATH)
        find_library(HIREDIS_LIBRARY NAMES hiredis
                HINTS "${HIREDIS_ROOT_DIR}/lib"
                NO_DEFAULT_PATH)
    else()
        find_path(HIREDIS_INCLUDE_DIR NAMES hiredis.h
                PATHS /usr/include/hiredis /usr/local/include/hiredis)
        find_library(HIREDIS_LIBRARY NAMES hiredis
                PATHS /usr/lib /usr/local/lib
                PATH_SUFFIXES x86_64-linux-gnu)
    endif()

    if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
        target_include_directories(MSTL PUBLIC ${HIREDIS_INCLUDE_DIR})
        target_link_libraries(MSTL PUBLIC ${HIREDIS_LIBRARY})
        target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_REDIS__)
        enable_database_support(MSTL)
        message(STATUS "Redis (hiredis) linked successfully")
    else()
        message(WARNING "Redis (hiredis) not found - Redis components will be unavailable")
    endif()
else()
    message(STATUS "Redis support disabled")
endif()

# ------------------- Qt6配置 -------------------
if(MSTL_ENABLE_QT6 AND MSTL_ARCH STREQUAL "x64" AND CMAKE_CXX_STANDARD GREATER_EQUAL 17)
    # 检查OpenGL依赖 (Linux)
    if(UNIX AND NOT APPLE)
        find_package(OpenGL QUIET)
        if(NOT OpenGL_FOUND)
            message(WARNING "OpenGL not found. Qt6 requires OpenGL support on Linux")
            set(QT6_SKIP TRUE)
        endif()
    endif()

    if(NOT QT6_SKIP)
        # 设置Qt6路径
        if(NOT DEFINED QT6_ROOT_DIR)
            if(MSVC)
                set(QT6_ROOT_DIR "D:/Qt/6.9.1/msvc2022_64")
            elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND WIN32)
                set(QT6_ROOT_DIR "D:/Qt/6.9.1/mingw_64")
            elseif(UNIX AND NOT APPLE)
                # 查找常见的Qt6安装路径
                set(POSSIBLE_QT6_PATHS
                        "/usr/lib/x86_64-linux-gnu/qt6"
                        "/usr/lib64/qt6"
                        "/usr/lib/qt6"
                        "/opt/qt6"
                        "/usr/local/qt6")

                foreach(QT_PATH ${POSSIBLE_QT6_PATHS})
                    if(EXISTS "${QT_PATH}/lib/cmake/Qt6")
                        set(QT6_ROOT_DIR "${QT_PATH}")
                        break()
                    endif()
                endforeach()
            elseif(APPLE)
                set(QT6_ROOT_DIR "/opt/homebrew/Cellar/qt@6")
            endif()

            set(QT6_ROOT_DIR "${QT6_ROOT_DIR}" CACHE PATH "Qt6 root directory")
        endif()

        # 添加Qt6到CMAKE_PREFIX_PATH
        if(EXISTS "${QT6_ROOT_DIR}")
            list(APPEND CMAKE_PREFIX_PATH "${QT6_ROOT_DIR}")
            if(EXISTS "${QT6_ROOT_DIR}/lib/cmake")
                list(APPEND CMAKE_PREFIX_PATH "${QT6_ROOT_DIR}/lib/cmake")
            endif()
        endif()

        find_package(Qt6 COMPONENTS Core Widgets Gui QUIET)

        if(Qt6_FOUND)
            # MSVC特定设置
            if(MSVC)
                target_compile_options(MSTL PRIVATE /Zc:__cplusplus)
            endif()

            # 启用Qt自动化工具
            set_target_properties(MSTL PROPERTIES
                    AUTOMOC ON
                    AUTOUIC ON
                    AUTORCC ON
            )

            target_link_libraries(MSTL PUBLIC Qt6::Core Qt6::Widgets Qt6::Gui)

            if(OpenGL_FOUND)
                target_link_libraries(MSTL PUBLIC OpenGL::GL)
            endif()

            target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_QT6__)
            message(STATUS "Qt6 linked successfully")
        else()
            message(WARNING "Qt6 not found. Install Qt6 or set correct QT6_ROOT_DIR")
            if(UNIX AND NOT APPLE)
                message(WARNING "  Ubuntu/Debian: sudo apt install qt6-base-dev")
                message(WARNING "  CentOS/RHEL: sudo dnf install qt6-qtbase-devel")
            endif()
        endif()
    endif()
else()
    set(disable_reasons "")
    if(NOT MSTL_ARCH STREQUAL "x64")
        list(APPEND disable_reasons "only x64 supported")
    endif()
    if(CMAKE_CXX_STANDARD LESS 17)
        list(APPEND disable_reasons "requires C++17+")
    endif()
    if(NOT MSTL_ENABLE_QT6)
        list(APPEND disable_reasons "disabled by option")
    endif()

    string(JOIN ", " reason_str ${disable_reasons})
    message(STATUS "Qt6 support disabled: ${reason_str}")
endif()

# ------------------- CUDA配置 -------------------
if(MSTL_ENABLE_CUDA AND MSTL_ARCH STREQUAL "x64")
    # 仅在MSVC下支持CUDA（根据原代码）
    if(MSVC AND DEFINED CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        find_package(CUDAToolkit QUIET)

        if(CUDAToolkit_FOUND)
            set_target_properties(MSTL PROPERTIES
                    CUDA_SEPARABLE_COMPILATION ON
                    CUDA_RESOLVE_DEVICE_SYMBOLS ON
            )
            target_link_libraries(MSTL PUBLIC CUDA::cudart)
            target_compile_definitions(MSTL PUBLIC MSTL_SUPPORT_CUDA__)
            message(STATUS "CUDA linked successfully")
        else()
            message(WARNING "CUDA Toolkit not found - CUDA components will be unavailable")
        endif()
    else()
        message(STATUS "CUDA support disabled: requires MSVC and CUDA compiler")
    endif()
else()
    message(STATUS "CUDA support disabled")
endif()

# ------------------- 安装配置 -------------------
include(GNUInstallDirs)

install(TARGETS MSTL
        EXPORT MSTLTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${ROOT_SRC_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mstl
)

install(FILES ${WEB_SRC_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mstl/web
)

# 生成配置文件用于find_package
install(EXPORT MSTLTargets
        FILE MSTLTargets.cmake
        NAMESPACE MSTL::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MSTL
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        MSTLConfigVersion.cmake
        VERSION 1.3.0
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/MSTLConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MSTLConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MSTL
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/MSTLConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/MSTLConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MSTL
)
