# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

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
CMAKE_COMMAND = /home/alon/Downloads/clion-2016.2.2/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/alon/Downloads/clion-2016.2.2/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/innometrics_run.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/innometrics_run.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/innometrics_run.dir/flags.make

CMakeFiles/innometrics_run.dir/main.cpp.o: CMakeFiles/innometrics_run.dir/flags.make
CMakeFiles/innometrics_run.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/innometrics_run.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/innometrics_run.dir/main.cpp.o -c /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/main.cpp

CMakeFiles/innometrics_run.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/innometrics_run.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/main.cpp > CMakeFiles/innometrics_run.dir/main.cpp.i

CMakeFiles/innometrics_run.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/innometrics_run.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/main.cpp -o CMakeFiles/innometrics_run.dir/main.cpp.s

CMakeFiles/innometrics_run.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/innometrics_run.dir/main.cpp.o.requires

CMakeFiles/innometrics_run.dir/main.cpp.o.provides: CMakeFiles/innometrics_run.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/innometrics_run.dir/build.make CMakeFiles/innometrics_run.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/innometrics_run.dir/main.cpp.o.provides

CMakeFiles/innometrics_run.dir/main.cpp.o.provides.build: CMakeFiles/innometrics_run.dir/main.cpp.o


# Object files for target innometrics_run
innometrics_run_OBJECTS = \
"CMakeFiles/innometrics_run.dir/main.cpp.o"

# External object files for target innometrics_run
innometrics_run_EXTERNAL_OBJECTS =

innometrics_run: CMakeFiles/innometrics_run.dir/main.cpp.o
innometrics_run: CMakeFiles/innometrics_run.dir/build.make
innometrics_run: InnoMetricsSources/libinnometrics_core.a
innometrics_run: /usr/lib/x86_64-linux-gnu/libSM.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libICE.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libX11.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXext.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXmu.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXi.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXext.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXmu.so
innometrics_run: /usr/lib/x86_64-linux-gnu/libXi.so
innometrics_run: CMakeFiles/innometrics_run.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable innometrics_run"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/innometrics_run.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/innometrics_run.dir/build: innometrics_run

.PHONY : CMakeFiles/innometrics_run.dir/build

CMakeFiles/innometrics_run.dir/requires: CMakeFiles/innometrics_run.dir/main.cpp.o.requires

.PHONY : CMakeFiles/innometrics_run.dir/requires

CMakeFiles/innometrics_run.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/innometrics_run.dir/cmake_clean.cmake
.PHONY : CMakeFiles/innometrics_run.dir/clean

CMakeFiles/innometrics_run.dir/depend:
	cd /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug /home/alon/ClionProjects/innometrics/LinuxMetricsCollector/InnoMetrics/cmake-build-debug/CMakeFiles/innometrics_run.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/innometrics_run.dir/depend
