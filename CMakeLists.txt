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
    "include/arasy/types/all.hpp"
    "include/arasy/types/base.hpp"
    "include/arasy/types/boolean.hpp"
    "include/arasy/types/cfunction.hpp"
    "include/arasy/types/integer.hpp"
    "include/arasy/types/nil.hpp"
    "include/arasy/types/number.hpp"
    "include/arasy/types/string.hpp"
    "include/arasy/types/thread.hpp"
    src/types.cpp
    src/thread.cpp
    "include/arasy/lua.hpp"
    "include/arasy/pushfmt.hpp"
    "include/arasy/utils.hpp"
    "include/arasy/errors.hpp"
    src/errors.cpp
    "include/arasy/registry.hpp"
    src/registry.cpp
)
target_include_directories(core PUBLIC "include" ${LUA_INCLUDE_DIR})
target_link_libraries(core PUBLIC ${LUA_LIBRARIES})

enable_testing()
add_executable(
    arasy_unit_tests
    tests/core.cpp
    tests/fmt.cpp
)
target_link_libraries(arasy_unit_tests PUBLIC core GTest::gtest_main)

file(GLOB all_test_scripts ${CMAKE_SOURCE_DIR}/tests/scripts/**)
add_custom_target(
    test_lua_scripts
    DEPENDS ${all_test_scripts}
)
add_custom_command(
    TARGET test_lua_scripts
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/scripts ./tests/scripts/
)

add_custom_target(
    run_unit_tests
    COMMAND $<TARGET_FILE:arasy_unit_tests> | tee tests/report.txt
    DEPENDS arasy_unit_tests
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(arasy_unit_tests)
