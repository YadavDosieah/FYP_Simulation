# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki

# Include any dependencies generated for this target.
include tests/CMakeFiles/testGeometry.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/testGeometry.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/testGeometry.dir/flags.make

tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.o: tests/CMakeFiles/testGeometry.dir/flags.make
tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.o: tests/testGeometry.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.o"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testGeometry.dir/testGeometry.cpp.o -c /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests/testGeometry.cpp

tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testGeometry.dir/testGeometry.cpp.i"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests/testGeometry.cpp > CMakeFiles/testGeometry.dir/testGeometry.cpp.i

tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testGeometry.dir/testGeometry.cpp.s"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests/testGeometry.cpp -o CMakeFiles/testGeometry.dir/testGeometry.cpp.s

# Object files for target testGeometry
testGeometry_OBJECTS = \
"CMakeFiles/testGeometry.dir/testGeometry.cpp.o"

# External object files for target testGeometry
testGeometry_EXTERNAL_OBJECTS =

tests/testGeometry: tests/CMakeFiles/testGeometry.dir/testGeometry.cpp.o
tests/testGeometry: tests/CMakeFiles/testGeometry.dir/build.make
tests/testGeometry: enki/libenki.a
tests/testGeometry: tests/CMakeFiles/testGeometry.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable testGeometry"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testGeometry.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/testGeometry.dir/build: tests/testGeometry

.PHONY : tests/CMakeFiles/testGeometry.dir/build

tests/CMakeFiles/testGeometry.dir/clean:
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests && $(CMAKE_COMMAND) -P CMakeFiles/testGeometry.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/testGeometry.dir/clean

tests/CMakeFiles/testGeometry.dir/depend:
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/tests/CMakeFiles/testGeometry.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/testGeometry.dir/depend

