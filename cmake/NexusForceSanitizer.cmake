# NexusForceSanitizer.cmake
# AddressSanitizer / UndefinedBehaviorSanitizer / ThreadSanitizer support.
#
# Usage:
#   cmake -B build -DNEXUSFORCE_ENABLE_ASAN=ON -DNEXUSFORCE_ENABLE_UBSAN=ON
#
# The flags are applied centrally by nexusforce_compiler_options(), so the
# library, tests, examples, benchmarks and tools always share one sanitizer
# configuration. Mixing sanitized and unsanitized translation units in one
# binary produces false negatives and startup failures, which is why this is
# not exposed as a per-target switch.
#
# The sanitizers are a developer/CI facility: they are never exported to
# downstream consumers through NexusForceConfig.cmake.

# Internal: the sanitizer flags requested by the cache options.
function(_nexusforce_sanitizer_flags out_var)
    set(_flags "")

    if(NEXUSFORCE_ENABLE_ASAN)
        list(APPEND _flags -fsanitize=address -fno-omit-frame-pointer)
    endif()

    if(NEXUSFORCE_ENABLE_UBSAN)
        # -fno-sanitize-recover makes undefined behaviour abort instead of
        # printing a warning and continuing, so the error is reported as a
        # non-zero exit status and can gate CI.
        list(APPEND _flags -fsanitize=undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
    endif()

    if(NEXUSFORCE_ENABLE_TSAN)
        list(APPEND _flags -fsanitize=thread -fno-omit-frame-pointer)
    endif()

    set(${out_var} ${_flags} PARENT_SCOPE)
endfunction()

# Internal: reject combinations that cannot be linked into one binary.
function(_nexusforce_validate_sanitizers)
    if(NEXUSFORCE_ENABLE_ASAN AND NEXUSFORCE_ENABLE_TSAN)
        message(FATAL_ERROR
                "NexusForce: NEXUSFORCE_ENABLE_ASAN and NEXUSFORCE_ENABLE_TSAN are mutually exclusive. "
                "AddressSanitizer and ThreadSanitizer cannot be linked into the same binary. "
                "Configure two separate build directories instead.")
    endif()

    if(NEXUSFORCE_ENABLE_ASAN OR NEXUSFORCE_ENABLE_UBSAN OR NEXUSFORCE_ENABLE_TSAN)
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(FATAL_ERROR
                    "NexusForce: sanitizers require GCC or Clang. "
                    "The current compiler is ${CMAKE_CXX_COMPILER_ID}, which does not support "
                    "-fsanitize. Sanitizer builds are therefore Linux/GCC-Clang only.")
        endif()

        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_CONFIGURATION_TYPES)
            message(WARNING
                    "NexusForce: sanitizers are enabled with CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'. "
                    "Debug is recommended so that inlined frames keep usable symbol information.")
        endif()
    endif()
endfunction()

# Internal: short human readable summary of the active sanitizers.
function(_nexusforce_sanitizer_summary out_var)
    set(_names "")
    if(NEXUSFORCE_ENABLE_ASAN)
        list(APPEND _names "ASan")
    endif()
    if(NEXUSFORCE_ENABLE_UBSAN)
        list(APPEND _names "UBSan")
    endif()
    if(NEXUSFORCE_ENABLE_TSAN)
        list(APPEND _names "TSan")
    endif()
    if(_names)
        list(JOIN _names "+" _joined)
        set(${out_var} "${_joined}" PARENT_SCOPE)
    else()
        set(${out_var} "OFF" PARENT_SCOPE)
    endif()
endfunction()

# Apply the requested sanitizers to a target.
# Both compile and link options are emitted: without the link option the binary
# fails to start because the sanitizer runtime is never pulled in.
function(nexusforce_sanitizer_options target)
    if(NOT NEXUSFORCE_ENABLE_ASAN AND NOT NEXUSFORCE_ENABLE_UBSAN AND NOT NEXUSFORCE_ENABLE_TSAN)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Already reported as a fatal error during validation; keep the function
        # safe for installed-package consumers.
        return()
    endif()

    _nexusforce_sanitizer_flags(SANITIZER_FLAGS)
    if(NOT SANITIZER_FLAGS)
        return()
    endif()

    target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS})
    target_link_options(${target} PRIVATE ${SANITIZER_FLAGS})
endfunction()
