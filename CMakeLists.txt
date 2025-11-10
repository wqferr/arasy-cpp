cmake_minimum_required(VERSION 3.18)
project(arasy LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LUA REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(
    core
    "include/arasy.hpp"
    src/arasy.cpp
)
target_include_directories(core PUBLIC "include" ${LUA_INCLUDE_DIR})
target_link_libraries(core PUBLIC ${LUA_LIBRARIES})

enable_testing()
add_executable(
    unit_tests
    tests/core.cpp
)
target_link_libraries(unit_tests PRIVATE core GTest::gtest_main)

add_custom_target(
    run_unit_tests
    COMMAND $<TARGET_FILE:unit_tests> | tee tests/report.txt
    DEPENDS unit_tests
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
