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
CMAKE_SOURCE_DIR = /home/morey/Desktop/MWENCLUBHOUSE/post

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/morey/Desktop/MWENCLUBHOUSE/post

# Include any dependencies generated for this target.
include CMakeFiles/lws-minimal-http-server-form-post-file.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lws-minimal-http-server-form-post-file.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lws-minimal-http-server-form-post-file.dir/flags.make

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o: CMakeFiles/lws-minimal-http-server-form-post-file.dir/flags.make
CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o: minimal-http-server-form-post-file.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/morey/Desktop/MWENCLUBHOUSE/post/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o   -c /home/morey/Desktop/MWENCLUBHOUSE/post/minimal-http-server-form-post-file.c

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/morey/Desktop/MWENCLUBHOUSE/post/minimal-http-server-form-post-file.c > CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.i

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/morey/Desktop/MWENCLUBHOUSE/post/minimal-http-server-form-post-file.c -o CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.s

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.requires:

.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.requires

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.provides: CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.requires
	$(MAKE) -f CMakeFiles/lws-minimal-http-server-form-post-file.dir/build.make CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.provides.build
.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.provides

CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.provides.build: CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o


# Object files for target lws-minimal-http-server-form-post-file
lws__minimal__http__server__form__post__file_OBJECTS = \
"CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o"

# External object files for target lws-minimal-http-server-form-post-file
lws__minimal__http__server__form__post__file_EXTERNAL_OBJECTS =

lws-minimal-http-server-form-post-file: CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o
lws-minimal-http-server-form-post-file: CMakeFiles/lws-minimal-http-server-form-post-file.dir/build.make
lws-minimal-http-server-form-post-file: CMakeFiles/lws-minimal-http-server-form-post-file.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/morey/Desktop/MWENCLUBHOUSE/post/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lws-minimal-http-server-form-post-file"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lws-minimal-http-server-form-post-file.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lws-minimal-http-server-form-post-file.dir/build: lws-minimal-http-server-form-post-file

.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/build

CMakeFiles/lws-minimal-http-server-form-post-file.dir/requires: CMakeFiles/lws-minimal-http-server-form-post-file.dir/minimal-http-server-form-post-file.c.o.requires

.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/requires

CMakeFiles/lws-minimal-http-server-form-post-file.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lws-minimal-http-server-form-post-file.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/clean

CMakeFiles/lws-minimal-http-server-form-post-file.dir/depend:
	cd /home/morey/Desktop/MWENCLUBHOUSE/post && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/morey/Desktop/MWENCLUBHOUSE/post /home/morey/Desktop/MWENCLUBHOUSE/post /home/morey/Desktop/MWENCLUBHOUSE/post /home/morey/Desktop/MWENCLUBHOUSE/post /home/morey/Desktop/MWENCLUBHOUSE/post/CMakeFiles/lws-minimal-http-server-form-post-file.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lws-minimal-http-server-form-post-file.dir/depend

