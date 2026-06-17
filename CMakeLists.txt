cmake_minimum_required(VERSION 3.24)

project(DeterministicLimitOrderBookMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library for matching engine components
add_library(me_core
    src/parser.cpp
)

target_include_directories(me_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Command-line executable
add_executable(me_parser
    src/main.cpp
)

target_link_libraries(me_parser
    PRIVATE
        me_core
)

# Testing
include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(parser_tests
    tests/parser_tests.cpp
)

target_link_libraries(parser_tests
    PRIVATE
        me_core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(parser_tests)
