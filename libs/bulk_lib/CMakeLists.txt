cmake_minimum_required(VERSION 3.12)

project(bulk_lib)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true src/*.h)

include_directories(include)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} INTERFACE
    src
    include
)

target_link_libraries(${PROJECT_NAME} PUBLIC coro_lib)

#add_subdirectory(tests) TODO: check