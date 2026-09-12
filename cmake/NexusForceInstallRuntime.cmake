# NexusForceInstallRuntime.cmake
# 在安装期为共享库补齐运行时依赖闭包。
#
# Windows 不存在 RPATH：加载器只搜索加载方模块自身所在的目录、系统目录与 PATH。vcpkg 的
# app-local deps 只把依赖复制到构建树，安装树中的 NexusForce.dll 因此缺失全部第三方依赖，
# 使安装前缀里的 NFRS.exe 与下游消费者的可执行文件都以 STATUS_DLL_NOT_FOUND
# (0xc0000135) 启动失败。Linux 侧依赖由共享库自身的 RPATH 解析，本模块不参与。
#
# 用法:
#   include(cmake/NexusForceInstallRuntime.cmake)
#   nexusforce_install_runtime_dependencies(NexusForce)

function(nexusforce_install_runtime_dependencies target)
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET ${target})
        message(FATAL_ERROR "nexusforce_install_runtime_dependencies: target '${target}' not found")
    endif()

    get_target_property(_nf_type ${target} TYPE)
    if(NOT _nf_type STREQUAL "SHARED_LIBRARY")
        return()
    endif()

    # 依赖解析必须发生在安装期：configure 期目标文件尚不存在。
    # 搜索目录按优先级排列，首选构建目录——vcpkg 的 app-local deps 已把依赖复制到库文件旁，
    # 之后是 vcpkg 的已安装目录，作为 app-local deps 被关闭时的回退。
    set(_nfrt_search_dirs
            "$<TARGET_FILE_DIR:${target}>"
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin"
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin"
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin"
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/bin"
    )

    install(CODE "
        if(POLICY CMP0207)
            cmake_policy(SET CMP0207 NEW)
        endif()

        file(GET_RUNTIME_DEPENDENCIES
            EXECUTABLES \"$<TARGET_FILE:${target}>\"
            DIRECTORIES ${_nfrt_search_dirs}
            RESOLVED_DEPENDENCIES_VAR _nfrt_resolved
            UNRESOLVED_DEPENDENCIES_VAR _nfrt_unresolved
            # 系统 DLL 与 VC 运行库由操作系统与 Visual C++ 可再发行组件提供，与 vcpkg 的
            # app-local deps 保持一致，安装包不复制它们。
            PRE_EXCLUDE_REGEXES \"api-ms-\" \"ext-ms-\" \"^[Vv][Cc][Rr][Uu][Nn][Tt][Ii][Mm][Ee]140\" \"^[Mm][Ss][Vv][Cc][Pp]140\"
            POST_EXCLUDE_REGEXES \".*[Ss]ystem32.*\"
        )

        if(_nfrt_resolved)
            file(INSTALL \${_nfrt_resolved} DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")
            list(LENGTH _nfrt_resolved _nfrt_count)
            message(STATUS \"Installed \${_nfrt_count} runtime dependencies into ${CMAKE_INSTALL_BINDIR}\")
        endif()

        foreach(_nfrt_dependency IN LISTS _nfrt_unresolved)
            message(WARNING \"NexusForce: unresolved runtime dependency: \${_nfrt_dependency}\")
        endforeach()
    ")
endfunction()
