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

# Main source files for each server mode
set(SERVER_PL_MAIN "src/main_pl.cpp")
set(SERVER_LF_MAIN "src/main_lf.cpp")

# Add executable for the Pipeline server
add_executable(server_pl ${SERVER_PL_MAIN} ${MODEL_SOURCES} ${NETWORK_SOURCES})

# Add executable for the Leader-Followers server
add_executable(server_lf ${SERVER_LF_MAIN} ${MODEL_SOURCES} ${NETWORK_SOURCES})

# Add executable for the tests
add_executable(tests ${TEST_SOURCES} ${MODEL_SOURCES})

# Include directories for tests
target_include_directories(tests PRIVATE src/Model_Test)

# Enable testing
enable_testing()
add_test(NAME RunTests COMMAND tests)

# Print status messages
message(STATUS "Pipeline server executable created: server_pl")
message(STATUS "Leader-Followers server executable created: server_lf")
message(STATUS "Tests executable created: tests")