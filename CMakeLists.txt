# Minimum required version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(OS_FinalProject)

# Use C++ standard 17
set(CMAKE_CXX_STANDARD 17)

# Include directories where header files (.hpp) are located
include_directories(src/Model)
include_directories(src/Network)
include_directories(Tests/Model_Tests)

# Source files for the main project (separated by directories)
set(SOURCES
        src/Model/Graph.cpp
        src/Model/MST.cpp
        src/Model/MSTFactory.cpp
        src/Network/Server.cpp
        src/Network/ActiveObject.cpp
        src/main.cpp
        # Add header files here
        src/Model/Graph.hpp
        src/Model/MST.hpp
        src/Model/MSTFactory.hpp
        src/Network/Server.hpp
        src/Network/ActiveObject.hpp
)

# Add the main executable
add_executable(OS_finalProject ${SOURCES})

# Source files for the tests
set(TEST_SOURCES
        src/Model_Test/MST_Tests.cpp
        src/Model/Graph.cpp
        src/Model/MST.cpp
        src/Model/MSTFactory.cpp
        src/Network/Server.cpp
        src/Network/ActiveObject.cpp
        # Add header files here for test executable
        src/Model/Graph.hpp
        src/Model/MST.hpp
        src/Model/MSTFactory.hpp
        src/Network/Server.hpp
)

# Add the test executable
add_executable(OS_finalProject_Tests ${TEST_SOURCES})

# Enable testing with doctest
enable_testing()
add_test(NAME GraphTests COMMAND OS_finalProject_Tests)