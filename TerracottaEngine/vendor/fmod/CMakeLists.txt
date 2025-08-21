cmake_minimum_required(VERSION 3.10)

project(FMOD)

# FMOD SDK
if(NOT DEFINED ENV{FMOD_SDK_DIR})
	message(FATAL_ERROR "Please download the FMOD SDK and set FMOD_SDK_DIR its root directory")
endif()
set(FMOD_SDK_DIR $ENV{FMOD_SDK_DIR})

# Get include directory
set(FMOD_INC_DIR
    "${FMOD_SDK_DIR}/api/core/inc"
    "${FMOD_SDK_DIR}/api/studio/inc"
)

# Determine which files we need
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_PROCESSOR_LOWERCASE)
message(STATUS "Using ${CMAKE_SYSTEM_PROCESSOR_LOWERCASE}")

if(WIN32) # Windows
  message(STATUS "FMOD - OS: Windows")

  # Determine files
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FMOD_STATIC_LIBS "fmodL_vc.lib" "fmodstudioL_vc.lib")
    set(FMOD_SHARED_LIBS "fmodL.dll" "fmodstudioL.dll")
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FMOD_STATIC_LIBS "fmod_vc.lib" "fmodstudio_vc.lib")
    set(FMOD_SHARED_LIBS "fmod.dll" "fmodstudio.dll")
  else()
    message(FATAL_ERROR "Unknown build type!")
  endif()

  # Determine CPU architecture
  if(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "x86_64|amd64|arm64|aarch64")
    set(ARCHITECTURE "x64")
  elseif(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "x86|arm")
    set(ARCHITECTURE "x86")
  else()
    message(FATAL_ERROR "Unknown system processor architecture!")
  endif()
elseif(UNIX) # Linux/Unix
  message(STATUS "FMOD - OS: Unix")

  # Determine files
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FMOD_SHARED_LIBS "libfmodL.so" "libfmodstudioL.so")
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FMOD_SHARED_LIBS "libfmod.so" "libfmodstudio.so")
  else()
    message(FATAL_ERROR "Unknown build type!")
  endif()

  # Determine CPU architecture
  if(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "arm")
    set(ARCHITECTURE "arm")
  elseif(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "arm64")
    set(ARCHITECTURE "arm64")
  elseif(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "x86_64")
    set(ARCHITECTURE "x86_64")
  elseif(CMAKE_SYSTEM_PROCESSOR_LOWERCASE MATCHES "x86")
    set(ARCHITECTURE "x86")
  else()
    message(FATAL_ERROR "Unknown system processor architecture!")
  endif()
else() # Other
  message(FATAL_ERROR "Unsupported platform")
endif()

# For .libs needed by the compiler (NEED SDK INSTALLED)
# The .dlls should be copied over before runtime
set(FMOD_LIB_DIR
  "${FMOD_SDK_DIR}/api/core/lib/${ARCHITECTURE}"
  "${FMOD_SDK_DIR}/api/studio/lib/${ARCHITECTURE}")

message(STATUS "FMOD - Current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "FMOD - SDK directory: ${FMOD_SDK_DIR}")
message(STATUS "FMOD - Inc directories: ${FMOD_INC_DIR}")
message(STATUS "FMOD - Lib directories: ${FMOD_LIB_DIR}")
message(STATUS "FMOD - STATIC LIBS: ${FMOD_STATIC_LIBS}")
message(STATUS "FMOD - SHARED LIBS: ${FMOD_SHARED_LIBS}")

# Link FMOD library (since its an interface it will link to libraries to be inherited)
add_library(fmod INTERFACE)

# Include FMOD headers
target_include_directories(fmod INTERFACE ${FMOD_INC_DIR})
# Let other CMake projects know where to look for the files
target_link_directories(fmod INTERFACE ${FMOD_LIB_DIR})

# Send the FMOD library files depending on platform
if(WIN32)
  target_link_libraries(fmod INTERFACE ${FMOD_STATIC_LIBS})
elseif(UNIX)
  target_link_libraries(fmod INTERFACE ${FMOD_SHARED_LIBS})
endif()