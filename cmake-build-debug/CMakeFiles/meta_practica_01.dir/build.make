# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Users\Administrator\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\232.9921.42\bin\cmake\win\x64\bin\cmake.exe

# The command to remove a file.
RM = C:\Users\Administrator\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\232.9921.42\bin\cmake\win\x64\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Administrator\Documents\meta-practica-01

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/meta_practica_01.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/meta_practica_01.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/meta_practica_01.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/meta_practica_01.dir/flags.make

CMakeFiles/meta_practica_01.dir/main.cpp.obj: CMakeFiles/meta_practica_01.dir/flags.make
CMakeFiles/meta_practica_01.dir/main.cpp.obj: C:/Users/Administrator/Documents/meta-practica-01/main.cpp
CMakeFiles/meta_practica_01.dir/main.cpp.obj: CMakeFiles/meta_practica_01.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/meta_practica_01.dir/main.cpp.obj"
	C:\Users\Administrator\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\232.9921.42\bin\mingw\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/meta_practica_01.dir/main.cpp.obj -MF CMakeFiles\meta_practica_01.dir\main.cpp.obj.d -o CMakeFiles\meta_practica_01.dir\main.cpp.obj -c C:\Users\Administrator\Documents\meta-practica-01\main.cpp

CMakeFiles/meta_practica_01.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/meta_practica_01.dir/main.cpp.i"
	C:\Users\Administrator\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\232.9921.42\bin\mingw\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Administrator\Documents\meta-practica-01\main.cpp > CMakeFiles\meta_practica_01.dir\main.cpp.i

CMakeFiles/meta_practica_01.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/meta_practica_01.dir/main.cpp.s"
	C:\Users\Administrator\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\232.9921.42\bin\mingw\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Administrator\Documents\meta-practica-01\main.cpp -o CMakeFiles\meta_practica_01.dir\main.cpp.s

# Object files for target meta_practica_01
meta_practica_01_OBJECTS = \
"CMakeFiles/meta_practica_01.dir/main.cpp.obj"

# External object files for target meta_practica_01
meta_practica_01_EXTERNAL_OBJECTS =

meta_practica_01.exe: CMakeFiles/meta_practica_01.dir/main.cpp.obj
meta_practica_01.exe: CMakeFiles/meta_practica_01.dir/build.make
meta_practica_01.exe: C:/Users/Administrator/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/232.9921.42/bin/mingw/lib/gcc/x86_64-w64-mingw32/13.1.0/libgomp.dll.a
meta_practica_01.exe: C:/Users/Administrator/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/232.9921.42/bin/mingw/x86_64-w64-mingw32/lib/libmingwthrd.a
meta_practica_01.exe: CMakeFiles/meta_practica_01.dir/linkLibs.rsp
meta_practica_01.exe: CMakeFiles/meta_practica_01.dir/objects1.rsp
meta_practica_01.exe: CMakeFiles/meta_practica_01.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable meta_practica_01.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\meta_practica_01.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/meta_practica_01.dir/build: meta_practica_01.exe
.PHONY : CMakeFiles/meta_practica_01.dir/build

CMakeFiles/meta_practica_01.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\meta_practica_01.dir\cmake_clean.cmake
.PHONY : CMakeFiles/meta_practica_01.dir/clean

CMakeFiles/meta_practica_01.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Administrator\Documents\meta-practica-01 C:\Users\Administrator\Documents\meta-practica-01 C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug C:\Users\Administrator\Documents\meta-practica-01\cmake-build-debug\CMakeFiles\meta_practica_01.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/meta_practica_01.dir/depend

