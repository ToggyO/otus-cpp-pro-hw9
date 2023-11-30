file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true src/*.h)

add_library(${LIB_BINARY} SHARED ${SOURCES})

set_target_properties(${LIB_BINARY} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_BINARY} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_subdirectory(tests)