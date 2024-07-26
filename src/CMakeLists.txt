file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_executable(${INSTALL_TARGET} ${SOURCES}
        ../async/orchestrator.h
        ../async/task_awaitable.h)

set_target_properties(${INSTALL_TARGET} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${INSTALL_TARGET} PUBLIC
    ${LIB_BINARY}
)