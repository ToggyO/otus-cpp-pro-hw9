cmake_minimum_required(VERSION 3.12)

project(coro_lib VERSION 1.0.1 DESCRIPTION "coroutines library") # TODO: версию пофиксить

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    VERSION ${PROJECT_VERSION}
    # PUBLIC_HEADER include/mylib.h
)

target_include_directories(${PROJECT_NAME} INTERFACE
    "."
)