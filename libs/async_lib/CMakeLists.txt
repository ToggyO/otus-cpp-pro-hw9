cmake_minimum_required(VERSION 3.9)

project(async)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} SHARED async.cpp async.h)

target_include_directories(${PROJECT_NAME} INTERFACE ".")

target_link_libraries(${PROJECT_NAME} PUBLIC bulk_lib)
