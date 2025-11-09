cmake_minimum_required(VERSION 3.18)
project(arasy LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LUA REQUIRED)
find_package(GTest CONFIG REQUIRED)

include_directories("include/")
include_directories(${LUA_INCLUDE_DIRS})

enable_testing()
add_executable(
    unit_tests
    tests/deleteme.cpp
)
target_link_libraries(unit_tests PRIVATE ${LUA_LIBRARY} GTest::gtest_main)

add_custom_command(
    TARGET unit_tests
    POST_BUILD
    COMMAND $<TARGET_FILE:unit_tests> | tee tests/report.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
