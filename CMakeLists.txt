# Minimum required version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(OS_FinalProject)

# Use C++ standard 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")

# Include directories where header files are located
include_directories(src/Model src/Network src/Model_Test)

# Add all source files from subdirectories (*.cpp) to the project
file(GLOB_RECURSE SOURCES_SUBDIRS "src/**/*.cpp")

# Add source files from the root src directory (e.g., src/main.cpp)
file(GLOB SOURCES_ROOT "src/*.cpp")

# Add all header files (*.hpp) for IDE support
file(GLOB_RECURSE HEADERS "src/**/*.hpp")

# Combine all sources into one list
set(SOURCES ${SOURCES_ROOT} ${SOURCES_SUBDIRS} ${HEADERS})

# Print detected source and header files (for debugging)
message(STATUS "Source files: ${SOURCES}")
message(STATUS "Header files: ${HEADERS}")

# Add the main executable for the project
add_executable(OS_FinalProject ${SOURCES})

# Enable testing (optional)
enable_testing()