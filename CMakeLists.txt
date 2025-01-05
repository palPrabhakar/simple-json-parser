cmake_minimum_required(VERSION 3.10)

project(simple-json-parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wswitch)

include_directories(include)

add_executable(sjp src/main.cpp src/parser.cpp src/tokenizer.cpp)

enable_testing()
find_package(GTest REQUIRED)
add_executable(test_parser test/parser_test.cpp src/parser.cpp src/tokenizer.cpp)
target_link_libraries(test_parser PRIVATE GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test_parser)

