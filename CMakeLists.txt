# Minimum required version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(OS_FinalProject)

# Use C++ standard 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")

# Include directories where header files (.hpp) are located
include_directories(src/Model src/Network src/Model_Tests)

# Set output directories for compiled files (equivalent to obj/ structure)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Define source files for main project
set(SOURCES
        src/Model/Graph.cpp
        src/Model/MSTFactory.cpp
        src/Network/Pipeline.cpp
        src/Network/ActiveObject.cpp
        src/Network/Server.cpp
        src/main.cpp
)

# Define header files for main project
set(HEADERS
        src/Model/Graph.hpp
        src/Model/MSTFactory.hpp
        src/Network/Pipeline.hpp
        src/Network/ActiveObject.hpp
        src/Network/Server.hpp
)

# Add the main executable for the project
add_executable(OS_FinalProject ${SOURCES} ${HEADERS})

# Define source files specifically for the test executable
set(TEST_SOURCES
        src/Model_Test/MST_Tests.cpp
        src/Model/Graph.cpp
        src/Model/MSTFactory.cpp
        src/Network/Pipeline.cpp
        src/Network/ActiveObject.cpp
        src/Network/Server.cpp
)

# Add the test executable with a unique name
add_executable(OS_FinalProject_Tests ${TEST_SOURCES})

# Enable testing with CTest (or doctest if configured)
enable_testing()
add_test(NAME MSTTests COMMAND OS_FinalProject_Tests)

# Clean command to match 'make clean'
add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_clean.cmake
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        COMMENT "Cleaning all build artifacts"
)