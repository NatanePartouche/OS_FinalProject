# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/CLion/CLion-2024.2.0.1-aarch64/clion-2024.2.0.1/bin/cmake/linux/aarch64/bin/cmake

# The command to remove a file.
RM = /opt/CLion/CLion-2024.2.0.1-aarch64/clion-2024.2.0.1/bin/cmake/linux/aarch64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/CLionProjects/OS_FinalProject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/server_LF.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/server_LF.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/server_LF.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server_LF.dir/flags.make

CMakeFiles/server_LF.dir/src/main.cpp.o: CMakeFiles/server_LF.dir/flags.make
CMakeFiles/server_LF.dir/src/main.cpp.o: /home/ubuntu/CLionProjects/OS_FinalProject/src/main.cpp
CMakeFiles/server_LF.dir/src/main.cpp.o: CMakeFiles/server_LF.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/server_LF.dir/src/main.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server_LF.dir/src/main.cpp.o -MF CMakeFiles/server_LF.dir/src/main.cpp.o.d -o CMakeFiles/server_LF.dir/src/main.cpp.o -c /home/ubuntu/CLionProjects/OS_FinalProject/src/main.cpp

CMakeFiles/server_LF.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/server_LF.dir/src/main.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/OS_FinalProject/src/main.cpp > CMakeFiles/server_LF.dir/src/main.cpp.i

CMakeFiles/server_LF.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/server_LF.dir/src/main.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/OS_FinalProject/src/main.cpp -o CMakeFiles/server_LF.dir/src/main.cpp.s

CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o: CMakeFiles/server_LF.dir/flags.make
CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o: /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/Graph.cpp
CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o: CMakeFiles/server_LF.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o -MF CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o.d -o CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o -c /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/Graph.cpp

CMakeFiles/server_LF.dir/src/Model/Graph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/server_LF.dir/src/Model/Graph.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/Graph.cpp > CMakeFiles/server_LF.dir/src/Model/Graph.cpp.i

CMakeFiles/server_LF.dir/src/Model/Graph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/server_LF.dir/src/Model/Graph.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/Graph.cpp -o CMakeFiles/server_LF.dir/src/Model/Graph.cpp.s

CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o: CMakeFiles/server_LF.dir/flags.make
CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o: /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/MSTFactory.cpp
CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o: CMakeFiles/server_LF.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o -MF CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o.d -o CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o -c /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/MSTFactory.cpp

CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/MSTFactory.cpp > CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.i

CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/OS_FinalProject/src/Model/MSTFactory.cpp -o CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.s

CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o: CMakeFiles/server_LF.dir/flags.make
CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o: /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/ActiveObject.cpp
CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o: CMakeFiles/server_LF.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o -MF CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o.d -o CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o -c /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/ActiveObject.cpp

CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/ActiveObject.cpp > CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.i

CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/ActiveObject.cpp -o CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.s

CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o: CMakeFiles/server_LF.dir/flags.make
CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o: /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/LeaderFollowers.cpp
CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o: CMakeFiles/server_LF.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o -MF CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o.d -o CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o -c /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/LeaderFollowers.cpp

CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/LeaderFollowers.cpp > CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.i

CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/OS_FinalProject/src/Network/LeaderFollowers.cpp -o CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.s

# Object files for target server_LF
server_LF_OBJECTS = \
"CMakeFiles/server_LF.dir/src/main.cpp.o" \
"CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o" \
"CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o" \
"CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o" \
"CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o"

# External object files for target server_LF
server_LF_EXTERNAL_OBJECTS =

server_LF: CMakeFiles/server_LF.dir/src/main.cpp.o
server_LF: CMakeFiles/server_LF.dir/src/Model/Graph.cpp.o
server_LF: CMakeFiles/server_LF.dir/src/Model/MSTFactory.cpp.o
server_LF: CMakeFiles/server_LF.dir/src/Network/ActiveObject.cpp.o
server_LF: CMakeFiles/server_LF.dir/src/Network/LeaderFollowers.cpp.o
server_LF: CMakeFiles/server_LF.dir/build.make
server_LF: CMakeFiles/server_LF.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable server_LF"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_LF.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server_LF.dir/build: server_LF
.PHONY : CMakeFiles/server_LF.dir/build

CMakeFiles/server_LF.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server_LF.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server_LF.dir/clean

CMakeFiles/server_LF.dir/depend:
	cd /home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/CLionProjects/OS_FinalProject /home/ubuntu/CLionProjects/OS_FinalProject /home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug /home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug /home/ubuntu/CLionProjects/OS_FinalProject/cmake-build-debug/CMakeFiles/server_LF.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/server_LF.dir/depend

