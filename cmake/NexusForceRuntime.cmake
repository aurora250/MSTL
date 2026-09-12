# NexusForceRuntime.cmake
# 提供 nexusforce_deploy_runtime() 函数，把 NexusForce 运行时库部署到目标构建目录。
#
# 用法:
#   find_package(NexusForce REQUIRED)
#   add_executable(my_app main.cpp)
#   target_link_libraries(my_app PRIVATE NexusForce::NexusForce)
#   nexusforce_deploy_runtime(my_app)

function(nexusforce_deploy_runtime target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "nexusforce_deploy_runtime: target '${target}' not found")
    endif()

    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET NexusForce::NexusForce)
        message(FATAL_ERROR "nexusforce_deploy_runtime: NexusForce::NexusForce target not found. "
                            "Ensure find_package(NexusForce) has been called.")
    endif()

    get_target_property(__nf_type NexusForce::NexusForce TYPE)
    if(NOT __nf_type STREQUAL "SHARED_LIBRARY" AND NOT __nf_type STREQUAL "UNKNOWN")
        return()
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:NexusForce::NexusForce>"
            "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Deploying NexusForce.dll for ${target}..."
        VERBATIM
    )

    if(NOT NEXUSFORCE_RUNTIME_DIR OR NOT EXISTS "${NEXUSFORCE_RUNTIME_DIR}")
        message(STATUS "nexusforce_deploy_runtime: ${NEXUSFORCE_RUNTIME_DIR} does not exist; only "
                       "NexusForce.dll is deployed. Reinstall NexusForce to package its third-party "
                       "runtime dependencies.")
        return()
    endif()

    file(GLOB __nf_runtime_dlls "${NEXUSFORCE_RUNTIME_DIR}/*.dll")

    set(__nf_dependencies "")
    foreach(__nf_dll IN LISTS __nf_runtime_dlls)
        get_filename_component(__nf_name "${__nf_dll}" NAME)
        if(NOT __nf_name MATCHES "^NexusForce")
            list(APPEND __nf_dependencies "${__nf_dll}")
        endif()
    endforeach()

    if(NOT __nf_dependencies)
        message(STATUS "nexusforce_deploy_runtime: ${NEXUSFORCE_RUNTIME_DIR} carries no third-party "
                       "runtime dependency; only NexusForce.dll is deployed. Reinstall NexusForce to "
                       "package them.")
        return()
    endif()

    list(LENGTH __nf_dependencies __nf_count)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${__nf_dependencies} "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Deploying ${__nf_count} NexusForce runtime dependencies for ${target}..."
        VERBATIM
    )
endfunction()
