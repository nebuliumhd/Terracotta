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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Terracotta")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories - unified approach for both multi-config and single-config generators
# Multi-config generators (Visual Studio, Xcode) will use $<CONFIG> automatically
# Single-config generators (Ninja, Makefiles) will use CMAKE_BUILD_TYPE

# For multi-config generators, this creates: out/x64-debug/bin/Debug/, out/x64-debug/bin/Release/, etc.
# For single-config generators, this creates: out/x64-debug/bin/Debug/ (when CMAKE_BUILD_TYPE=Debug)

if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    message(STATUS "Using multi-config generator")
    foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
        
        # EXEs and shared libraries (.dll, .so) go into /bin/{CONFIG}
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
        # Shared libraries (.dll, .so) go into /bin/{CONFIG} with the executable
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
        # Static libraries (.lib, .a) go into /bin/{CONFIG}/lib
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}/lib)
        # PDB symbols go into /bin/{CONFIG}/pdb
        set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}/pdb)
        
        message(STATUS "  ${CONFIG_TYPE}: ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}")
    endforeach()
else()
    # Single-config generators (Ninja, Makefiles)
    message(STATUS "Using single-config generator: ${CMAKE_BUILD_TYPE}")
    
    # EXEs and shared libraries (.dll, .so) go into /bin/{CMAKE_BUILD_TYPE}
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    # Shared libraries go into /bin/{CMAKE_BUILD_TYPE} with the executable
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    # Static libraries go into /bin/{CMAKE_BUILD_TYPE}/lib
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/lib)
    # PDB symbols go into /bin/{CMAKE_BUILD_TYPE}/pdb
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/pdb)
    
    message(STATUS "  Output: ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "Main CMakeLists.txt")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")

# Build external libraries first
add_subdirectory("TerracottaEngine/vendor/imgui-docking")
add_subdirectory("TerracottaEngine/vendor/glfw")
add_subdirectory("TerracottaEngine/vendor/glm")
add_subdirectory("TerracottaEngine/vendor/glad")
add_subdirectory("TerracottaEngine/vendor/spdlog")
add_subdirectory("TerracottaEngine/vendor/stb")
add_subdirectory("TerracottaEngine/vendor/openal-soft")

# Build game first
add_subdirectory("TerracottaGame")
# Then build the engine.
add_subdirectory("TerracottaEngine")