set(TEST_PROJECT_ROOT .)
set(TESTED_EXECUTABLE_PATH ${CMAKE_BINARY_DIR}/src/${INSTALL_TARGET})

configure_file(config.h.in config.h @ONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(TEST_SOURCES bulk_tests_unix.cpp)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(TEST_SOURCES bulk_tests_unix.cpp)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(FATAL_ERROR "Unsupported OS - '${CMAKE_SYSTEM_NAME}'")
else()
    message(FATAL_ERROR "Unsupported OS - '${CMAKE_SYSTEM_NAME}'")
endif()

#find_package(GTest REQUIRED)

enable_testing()

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests
        PRIVATE
        ../src
)
target_link_libraries(tests PUBLIC
#        GTest::gtest
#        GTest::gtest_main
#        ${INSTALL_TARGET_LIB_BINARY}
)

#include(GoogleTest)
#gtest_discover_tests(tests)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})