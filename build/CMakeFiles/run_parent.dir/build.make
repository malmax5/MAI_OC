# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = "C:/Program Files/CMake/bin/cmake.exe"

# The command to remove a file.
RM = "C:/Program Files/CMake/bin/cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1/build

# Utility rule file for run_parent.

# Include any custom commands dependencies for this target.
include CMakeFiles/run_parent.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/run_parent.dir/progress.make

CMakeFiles/run_parent: lab_1.exe
	"C:/Program Files/CMake/bin/cmake.exe" -E env ARGS="file.txt" C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1/build/lab_1.exe

run_parent: CMakeFiles/run_parent
run_parent: CMakeFiles/run_parent.dir/build.make
.PHONY : run_parent

# Rule to build all files generated by this target.
CMakeFiles/run_parent.dir/build: run_parent
.PHONY : CMakeFiles/run_parent.dir/build

CMakeFiles/run_parent.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run_parent.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run_parent.dir/clean

CMakeFiles/run_parent.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1 C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1 C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1/build C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1/build C:/Users/CrazyBlackFire/Documents/Code/OC/Lab1/build/CMakeFiles/run_parent.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run_parent.dir/depend

