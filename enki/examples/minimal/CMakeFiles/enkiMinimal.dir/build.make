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
include examples/minimal/CMakeFiles/enkiMinimal.dir/depend.make

# Include the progress variables for this target.
include examples/minimal/CMakeFiles/enkiMinimal.dir/progress.make

# Include the compile flags for this target's objects.
include examples/minimal/CMakeFiles/enkiMinimal.dir/flags.make

examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o: examples/minimal/CMakeFiles/enkiMinimal.dir/flags.make
examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o: examples/minimal/enkiMinimal.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o -c /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal/enkiMinimal.cpp

examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.i"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal/enkiMinimal.cpp > CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.i

examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.s"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal/enkiMinimal.cpp -o CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.s

# Object files for target enkiMinimal
enkiMinimal_OBJECTS = \
"CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o"

# External object files for target enkiMinimal
enkiMinimal_EXTERNAL_OBJECTS =

examples/minimal/enkiMinimal: examples/minimal/CMakeFiles/enkiMinimal.dir/enkiMinimal.cpp.o
examples/minimal/enkiMinimal: examples/minimal/CMakeFiles/enkiMinimal.dir/build.make
examples/minimal/enkiMinimal: enki/libenki.a
examples/minimal/enkiMinimal: examples/minimal/CMakeFiles/enkiMinimal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable enkiMinimal"
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/enkiMinimal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/minimal/CMakeFiles/enkiMinimal.dir/build: examples/minimal/enkiMinimal

.PHONY : examples/minimal/CMakeFiles/enkiMinimal.dir/build

examples/minimal/CMakeFiles/enkiMinimal.dir/clean:
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal && $(CMAKE_COMMAND) -P CMakeFiles/enkiMinimal.dir/cmake_clean.cmake
.PHONY : examples/minimal/CMakeFiles/enkiMinimal.dir/clean

examples/minimal/CMakeFiles/enkiMinimal.dir/depend:
	cd /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal /home/yadav/Documents/Final-Year-Project/FYP_Simulation/enki/examples/minimal/CMakeFiles/enkiMinimal.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/minimal/CMakeFiles/enkiMinimal.dir/depend

