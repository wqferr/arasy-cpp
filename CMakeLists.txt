cmake_minimum_required(VERSION 3.18)
project(arasy LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LUA REQUIRED)
find_package(GTest CONFIG)
find_package(Doxygen)

add_library(
    arasy
    "include/arasy.hpp"
    src/arasy.cpp
    "include/arasy/types.hpp"
    "include/arasy/types/base.hpp"
    "include/arasy/types/boolean.hpp"
    "include/arasy/types/function.hpp"
    "include/arasy/types/cfunction.hpp"
    "include/arasy/types/integer.hpp"
    "include/arasy/types/nil.hpp"
    "include/arasy/types/number.hpp"
    "include/arasy/types/string.hpp"
    "include/arasy/types/table.hpp"
    "include/arasy/types/thread.hpp"
    "include/arasy/types/lightuserdata.hpp"
    src/types.cpp
    src/indexable.cpp
    src/thread.cpp
    "include/arasy/lua.hpp"
    "include/arasy/pushfmt.hpp"
    "include/arasy/utils.hpp"
    "include/arasy/errors.hpp"
    src/errors.cpp
    "include/arasy/registry.hpp"
    src/registry.cpp
    "include/arasy/registry/reference.hpp"
    src/reference.cpp
    "include/arasy/module.hpp"
    src/module.cpp
)
target_include_directories(arasy PUBLIC "include" ${LUA_INCLUDE_DIR})

if(${Doxygen_FOUND})
    doxygen_add_docs(
        docs
        CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    )
endif()

if(MSVC)
    target_compile_options(arasy PRIVATE /W4)
else()
    target_compile_options(arasy -Wpedantic -Wextra)
endif()

target_link_libraries(arasy PUBLIC ${LUA_LIBRARIES})

enable_testing()
file(GLOB all_test_scripts ${CMAKE_CURRENT_SOURCE_DIR}/tests/scripts/**)
add_custom_target(
    test_lua_scripts
    DEPENDS ${all_test_scripts}
)
add_custom_command(
    TARGET test_lua_scripts
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/scripts/ ./tests/scripts/
)

if(${GTest_FOUND})
    add_executable(
        arasy_unit_tests
        tests/core.cpp
        tests/fmt.cpp
        tests/registry.cpp
        tests/table.cpp
        tests/functions.cpp
        tests/thread.cpp
        tests/module.cpp
    )
    target_link_libraries(arasy_unit_tests PUBLIC arasy GTest::gtest_main)
    add_dependencies(arasy_unit_tests test_lua_scripts)

    add_custom_target(
        run_unit_tests
        COMMAND $<TARGET_FILE:arasy_unit_tests> | tee tests/report.txt
        DEPENDS arasy_unit_tests
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    include(GoogleTest)
    gtest_discover_tests(arasy_unit_tests)
endif()
