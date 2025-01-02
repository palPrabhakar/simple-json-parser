cmake_minimum_required(VERSION 3.10)

project(simple-json-parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(sjp ${SOURCES})
