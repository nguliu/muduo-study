# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/muduo/4_muduo_study_again

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/muduo/4_muduo_study_again/build-debug-cpp11

# Include any dependencies generated for this target.
include examples/five_simple_examples/CMakeFiles/simple_daytime.dir/depend.make

# Include the progress variables for this target.
include examples/five_simple_examples/CMakeFiles/simple_daytime.dir/progress.make

# Include the compile flags for this target's objects.
include examples/five_simple_examples/CMakeFiles/simple_daytime.dir/flags.make

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/flags.make
examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o: ../examples/five_simple_examples/daytime/daytime.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/muduo/4_muduo_study_again/build-debug-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o -c /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/daytime.cc

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simple_daytime.dir/daytime/daytime.cc.i"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/daytime.cc > CMakeFiles/simple_daytime.dir/daytime/daytime.cc.i

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simple_daytime.dir/daytime/daytime.cc.s"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/daytime.cc -o CMakeFiles/simple_daytime.dir/daytime/daytime.cc.s

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.requires:

.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.requires

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.provides: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.requires
	$(MAKE) -f examples/five_simple_examples/CMakeFiles/simple_daytime.dir/build.make examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.provides.build
.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.provides

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.provides.build: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o


examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/flags.make
examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o: ../examples/five_simple_examples/daytime/main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/muduo/4_muduo_study_again/build-debug-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/simple_daytime.dir/daytime/main.cc.o -c /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/main.cc

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simple_daytime.dir/daytime/main.cc.i"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/main.cc > CMakeFiles/simple_daytime.dir/daytime/main.cc.i

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simple_daytime.dir/daytime/main.cc.s"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/muduo/4_muduo_study_again/examples/five_simple_examples/daytime/main.cc -o CMakeFiles/simple_daytime.dir/daytime/main.cc.s

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.requires:

.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.requires

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.provides: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.requires
	$(MAKE) -f examples/five_simple_examples/CMakeFiles/simple_daytime.dir/build.make examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.provides.build
.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.provides

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.provides.build: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o


# Object files for target simple_daytime
simple_daytime_OBJECTS = \
"CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o" \
"CMakeFiles/simple_daytime.dir/daytime/main.cc.o"

# External object files for target simple_daytime
simple_daytime_EXTERNAL_OBJECTS =

../bin/simple_daytime: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o
../bin/simple_daytime: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o
../bin/simple_daytime: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/build.make
../bin/simple_daytime: lib/libmuduo_net.a
../bin/simple_daytime: lib/libmuduo_base.a
../bin/simple_daytime: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/muduo/4_muduo_study_again/build-debug-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../../bin/simple_daytime"
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_daytime.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/five_simple_examples/CMakeFiles/simple_daytime.dir/build: ../bin/simple_daytime

.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/build

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/requires: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/daytime.cc.o.requires
examples/five_simple_examples/CMakeFiles/simple_daytime.dir/requires: examples/five_simple_examples/CMakeFiles/simple_daytime.dir/daytime/main.cc.o.requires

.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/requires

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/clean:
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples && $(CMAKE_COMMAND) -P CMakeFiles/simple_daytime.dir/cmake_clean.cmake
.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/clean

examples/five_simple_examples/CMakeFiles/simple_daytime.dir/depend:
	cd /home/muduo/4_muduo_study_again/build-debug-cpp11 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/muduo/4_muduo_study_again /home/muduo/4_muduo_study_again/examples/five_simple_examples /home/muduo/4_muduo_study_again/build-debug-cpp11 /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples /home/muduo/4_muduo_study_again/build-debug-cpp11/examples/five_simple_examples/CMakeFiles/simple_daytime.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/five_simple_examples/CMakeFiles/simple_daytime.dir/depend

