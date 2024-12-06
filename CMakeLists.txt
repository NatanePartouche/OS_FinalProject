# Minimum required version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(OS_FinalProject)

# Use C++ standard 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")

# Include directories
include_directories(src/Model src/Network src/Model_Test)

# Group common sources
file(GLOB_RECURSE MODEL_SOURCES
        "src/Model/*.cpp"
        "src/Model/*.hpp"
)
file(GLOB_RECURSE NETWORK_SOURCES
        "src/Network/*.cpp"
        "src/Network/*.hpp"

)
file(GLOB_RECURSE TEST_SOURCES
        "src/Model_Test/*.cpp"
        "src/Model_Test/*.hpp"

)

# Main source file
set(MAIN_SOURCE "src/main.cpp")

# Add executable for Pipeline mode
add_executable(server_PL ${MAIN_SOURCE} ${MODEL_SOURCES} ${NETWORK_SOURCES})
target_compile_definitions(server_PL PRIVATE DEFAULT_MODE="-PL" DEFAULT_PORT=8080 PL_MODE)

# Add executable for Leader-Followers mode
add_executable(server_LF ${MAIN_SOURCE} ${MODEL_SOURCES} ${NETWORK_SOURCES})
target_compile_definitions(server_LF PRIVATE DEFAULT_MODE="-LF" DEFAULT_PORT=8080 LF_MODE)

# Add executable for tests
add_executable(tests ${TEST_SOURCES} ${MODEL_SOURCES} ${NETWORK_SOURCES})
target_include_directories(tests PRIVATE src/Model_Test)

# Add executable for MST_Tests
file(GLOB_RECURSE MST_TEST_SOURCES
        "src/Model_Test/*.cpp"
        "src/Model_Test/*.h"

)
add_executable(mst_tests ${MST_TEST_SOURCES} ${MODEL_SOURCES} ${NETWORK_SOURCES})
target_include_directories(mst_tests PRIVATE src/Model_Test/MST_Tests)

# Enable testing
enable_testing()
add_test(NAME RunTests COMMAND ./tests)
add_test(NAME MST_Tests COMMAND ./mst_tests)

# Messages de confirmation
message(STATUS "Pipeline server executable created: server_PL")
message(STATUS "Leader-Followers server executable created: server_LF")
message(STATUS "Tests executable created: tests")
message(STATUS "MST_Tests executable created: mst_tests")