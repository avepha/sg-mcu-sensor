# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor/cmake-build-v2

# Utility rule file for PLATFORMIO_DEVICE_LIST.

# Include the progress variables for this target.
include CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/progress.make

CMakeFiles/PLATFORMIO_DEVICE_LIST:
	cd /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor && /usr/local/bin/pio -f -c clion device list

PLATFORMIO_DEVICE_LIST: CMakeFiles/PLATFORMIO_DEVICE_LIST
PLATFORMIO_DEVICE_LIST: CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/build.make

.PHONY : PLATFORMIO_DEVICE_LIST

# Rule to build all files generated by this target.
CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/build: PLATFORMIO_DEVICE_LIST

.PHONY : CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/build

CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/clean

CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/depend:
	cd /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor/cmake-build-v2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor/cmake-build-v2 /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor/cmake-build-v2 /Users/alfarie/Documents/Grobot-Project/@Project/sg-lts/mcu/sg-mcu-sensor/cmake-build-v2/CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PLATFORMIO_DEVICE_LIST.dir/depend

