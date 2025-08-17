# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# For vendor libraries
set(GLFW_BUILD_SHARED_LIBS ON)

project ("Terracotta")

# Check if we are using a multi-config generator (Visual Studio, Xcode)
if(CMAKE_CONFIGURATION_TYPES)
    foreach(CONFIG_TYPE Debug Release RelWithDebInfo MinSizeRel)
        # EXEs and shared libraries (.dll, .so) go into /bin/{CONFIG}
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
		# Shared libarires (.dll, .so) go into /bin/{CONFIG} with the executable
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
        # Static libraries (.lib, .a) go into /bin/{CONFIG}/lib
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}/lib)
		# Set PDB symbols
		set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}/pdb)
	endforeach()
else()
    # Single-config generators (Ninja, Makefiles)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
	# Shared libraries go into /bin/{CMAKE_BUILD_TYPE} with the executable
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    # Static libraries go into /bin/{CMAKE_BUILD_TYPE}/lib
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/lib)
    # PDB symbols go into /bin/{CMAKE_BUILD_TYPE}/pdb
	set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/pdb)
endif()

# Build game first
add_subdirectory ("TerracottaGame")
# Then build the engine.
add_subdirectory ("TerracottaEngine")