cmake_minimum_required(VERSION 3.18)
project(arasy LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LUA REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(
    core
    src/arasy.cpp
    src/types.cpp
)
target_include_directories(core PUBLIC "include" ${LUA_INCLUDE_DIR})
target_link_libraries(core PUBLIC ${LUA_LIBRARIES})

enable_testing()
add_executable(
    arasy_unit_tests
    tests/core.cpp
)
target_link_libraries(arasy_unit_tests PRIVATE core GTest::gtest GTest::gtest_main)

add_custom_target(
    run_unit_tests
    COMMAND $<TARGET_FILE:arasy_unit_tests> | tee tests/report.txt
    DEPENDS arasy_unit_tests
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(arasy_unit_tests)
