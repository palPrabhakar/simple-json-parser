cmake_minimum_required(VERSION 3.15)

project(simple-json-parser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(sjp STATIC src/parser.cpp src/tokenizer.cpp)
target_compile_options(sjp PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wswitch -O2)
target_include_directories(sjp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(main src/main.cpp)
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wswitch -O2)
    target_link_libraries(main PRIVATE sjp)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(test_parser test/parser_test.cpp)
    target_link_libraries(test_parser PRIVATE sjp GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(test_parser)
endif()

# Export the library for FetchContent or find_package
export(TARGETS sjp FILE sjp-config.cmake)

