add_executable(${INSTALL_TARGET} main.cpp)

target_link_libraries(${INSTALL_TARGET} PRIVATE async)

set_target_properties(${INSTALL_TARGET} PROPERTIES LINKER_LANGUAGE CXX)