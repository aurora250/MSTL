if(NOT MSTL_BUILD_TESTS)
    return()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_SOURCES main.cpp try.cpp try.h)
add_executable(MSTL_TEST ${TEST_SOURCES})

target_link_libraries(MSTL_TEST PRIVATE MSTL::MSTL)
target_include_directories(MSTL_TEST PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

configure_compiler_options(MSTL_TEST)
