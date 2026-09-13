# NexusForceInstallRuntime.cmake
# 在安装期为共享库补齐运行时依赖闭包。
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

    get_target_property(_nfrt_type ${target} TYPE)
    if(NOT _nfrt_type STREQUAL "SHARED_LIBRARY")
        return()
    endif()

    set(_nfrt_search_dirs "$<TARGET_FILE_DIR:${target}>")

    get_target_property(_nfrt_links ${target} INTERFACE_LINK_LIBRARIES)
    if(NOT _nfrt_links)
        get_target_property(_nfrt_links ${target} LINK_LIBRARIES)
    endif()

    set(_nfrt_pending ${_nfrt_links})
    set(_nfrt_visited "")
    while(_nfrt_pending)
        list(POP_FRONT _nfrt_pending _nfrt_link)
        if(NOT TARGET ${_nfrt_link})
            continue()
        endif()
        if(_nfrt_link IN_LIST _nfrt_visited)
            continue()
        endif()
        list(APPEND _nfrt_visited "${_nfrt_link}")

        get_target_property(_nfrt_link_type ${_nfrt_link} TYPE)
        if(_nfrt_link_type STREQUAL "SHARED_LIBRARY")
            foreach(_nfrt_property IN ITEMS
                    IMPORTED_LOCATION IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION_RELEASE
                    IMPORTED_LOCATION_RELWITHDEBINFO IMPORTED_LOCATION_MINSIZEREL
                    IMPORTED_IMPLIB IMPORTED_IMPLIB_DEBUG IMPORTED_IMPLIB_RELEASE
                    IMPORTED_IMPLIB_RELWITHDEBINFO IMPORTED_IMPLIB_MINSIZEREL)
                get_target_property(_nfrt_value ${_nfrt_link} ${_nfrt_property})
                if(_nfrt_value AND NOT _nfrt_value MATCHES "-NOTFOUND$")
                    get_filename_component(_nfrt_value_dir "${_nfrt_value}" DIRECTORY)
                    list(APPEND _nfrt_search_dirs "${_nfrt_value_dir}")
                endif()
            endforeach()
        endif()

        get_target_property(_nfrt_children ${_nfrt_link} INTERFACE_LINK_LIBRARIES)
        if(_nfrt_children)
            list(APPEND _nfrt_pending ${_nfrt_children})
        endif()
    endwhile()

    file(GLOB _nfrt_triplets "${CMAKE_BINARY_DIR}/vcpkg_installed/*")
    foreach(_nfrt_triplet IN LISTS _nfrt_triplets)
        if(IS_DIRECTORY "${_nfrt_triplet}")
            list(APPEND _nfrt_search_dirs "${_nfrt_triplet}/bin" "${_nfrt_triplet}/debug/bin")
        endif()
    endforeach()

    list(APPEND _nfrt_search_dirs
            "$CACHE{VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin"
            "$CACHE{VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin"
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin"
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
    list(REMOVE_DUPLICATES _nfrt_search_dirs)

    install(CODE "
        if(POLICY CMP0207)
            cmake_policy(SET CMP0207 NEW)
        endif()

        file(GET_RUNTIME_DEPENDENCIES
            EXECUTABLES \"$<TARGET_FILE:${target}>\"
            DIRECTORIES ${_nfrt_search_dirs}
            RESOLVED_DEPENDENCIES_VAR _nfrt_resolved
            UNRESOLVED_DEPENDENCIES_VAR _nfrt_unresolved
            PRE_EXCLUDE_REGEXES \"api-ms-\" \"ext-ms-\" \"^[Vv][Cc][Rr][Uu][Nn][Tt][Ii][Mm][Ee]140\" \"^[Mm][Ss][Vv][Cc][Pp]140\"
            POST_EXCLUDE_REGEXES \".*[Ss]ystem32.*\"
        )

        set(_nfrt_install \${_nfrt_resolved})
        set(_nfrt_missing \"\")
        foreach(_nfrt_name IN LISTS _nfrt_unresolved)
            string(TOLOWER \"\${_nfrt_name}\" _nfrt_lower_name)
            set(_nfrt_match \"\")
            foreach(_nfrt_dir IN LISTS _nfrt_search_dirs)
                if(NOT IS_DIRECTORY \"\${_nfrt_dir}\")
                    continue()
                endif()
                file(GLOB _nfrt_entries \"\${_nfrt_dir}/*.dll\")
                foreach(_nfrt_entry IN LISTS _nfrt_entries)
                    get_filename_component(_nfrt_entry_name \"\${_nfrt_entry}\" NAME)
                    string(TOLOWER \"\${_nfrt_entry_name}\" _nfrt_lower_entry)
                    if(_nfrt_lower_entry STREQUAL _nfrt_lower_name)
                        set(_nfrt_match \"\${_nfrt_entry}\")
                        break()
                    endif()
                endforeach()
                if(_nfrt_match)
                    break()
                endif()
            endforeach()
            if(_nfrt_match)
                list(APPEND _nfrt_install \"\${_nfrt_match}\")
            else()
                list(APPEND _nfrt_missing \"\${_nfrt_name}\")
            endif()
        endforeach()

        if(_nfrt_install)
            file(INSTALL \${_nfrt_install} DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")
            list(LENGTH _nfrt_install _nfrt_count)
            message(STATUS \"Installed \${_nfrt_count} runtime dependencies into ${CMAKE_INSTALL_BINDIR}\")
        endif()

        if(_nfrt_missing)
            message(WARNING \"NexusForce: runtime dependencies not found: \${_nfrt_missing}\")
            message(WARNING \"NexusForce: searched directories: \${_nfrt_search_dirs}\")
        endif()
    ")
endfunction()
