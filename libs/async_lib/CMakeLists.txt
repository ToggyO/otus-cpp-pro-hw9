cmake_minimum_required(VERSION 3.9)

project(async)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} SHARED async.cpp async.h)

target_link_libraries(${PROJECT_NAME} PRIVATE bulk_lib)
