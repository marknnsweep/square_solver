cmake_minimum_required(VERSION 3.10)

project(MyProject LANGUAGES CXX)

execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (GCC_VERSION VERSION_LESS 12)
    message(FATAL_ERROR "G++ version must be at least 12! Found: ${GCC_VERSION}")
endif()

find_package(Python3 3.7 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_executable(quadratic_solver ${SOURCES})

set(TESTS_FILE "${CMAKE_SOURCE_DIR}/tests/tests.py")
set(DESTINATION_DIR "${CMAKE_BINARY_DIR}")

add_custom_target(copy_tests
    COMMAND ${CMAKE_COMMAND} -E copy ${TESTS_FILE} ${DESTINATION_DIR}
    COMMENT "Copying tests.py to build directory..."
)

add_dependencies(quadratic_solver copy_tests)

target_compile_options(quadratic_solver PRIVATE --std=c++17)