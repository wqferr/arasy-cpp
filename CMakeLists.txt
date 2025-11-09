cmake_minimum_required(VERSION 3.18)
project(arasy LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(lua REQUIRED CONFIG)

include_directories("include/")
include_directories(${lua_INCLUDE_DIRS})

add_executable(
    test_compilation
    src/main.cpp
)
target_link_libraries(test_compilation lua::lua)
