cmake_minimum_required(VERSION 3.28)
project(libkpnet C)

set(PROJECT_NAME libkpnet)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_custom_target(run_with_sudo
        COMMAND sudo src/src
        DEPENDS src
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

add_subdirectory(src)
