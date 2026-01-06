cmake_minimum_required(VERSION 3.10)

project(OpenAL-Precompiled)

# Determine architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OPENAL_ARCH "x64")
    message(STATUS "OpenAL: Using x64 architecture")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(OPENAL_ARCH "x86")
    message(STATUS "OpenAL: Using x86 architecture")
else()
    message(FATAL_ERROR "OpenAL: Unsupported architecture")
endif()

# Set paths to precompiled libraries
set(OPENAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${OPENAL_ARCH})
set(OPENAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find the appropriate library file based on platform
if(WIN32)
    set(OPENAL_LIB_FILE ${OPENAL_LIB_DIR}/OpenAL32.lib)
    set(OPENAL_DLL_FILE ${OPENAL_LIB_DIR}/OpenAL32.dll)
elseif(UNIX AND NOT APPLE)
    # Linux
    set(OPENAL_LIB_FILE ${OPENAL_LIB_DIR}/libopenal.so)
elseif(APPLE)
    # macOS
    set(OPENAL_LIB_FILE ${OPENAL_LIB_DIR}/libopenal.dylib)
endif()

# Verify the library exists
if(NOT EXISTS ${OPENAL_LIB_FILE})
    message(FATAL_ERROR "OpenAL library not found at: ${OPENAL_LIB_FILE}")
endif()

# Create an imported library target
add_library(OpenAL SHARED IMPORTED GLOBAL)

# Set the imported location
set_target_properties(OpenAL PROPERTIES
    IMPORTED_LOCATION ${OPENAL_LIB_FILE}
    INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR}
)

# On Windows, set the import library (.lib) separately and copy DLL
if(WIN32)
    if(NOT EXISTS ${OPENAL_DLL_FILE})
        message(FATAL_ERROR "OpenAL DLL not found at: ${OPENAL_DLL_FILE}")
    endif()
    
    set_target_properties(OpenAL PROPERTIES
        IMPORTED_IMPLIB ${OPENAL_LIB_FILE}
        IMPORTED_LOCATION ${OPENAL_DLL_FILE}
    )
    
    message(STATUS "OpenAL: Import library: ${OPENAL_LIB_FILE}")
    message(STATUS "OpenAL: DLL location: ${OPENAL_DLL_FILE}")
    
    # Copy DLL to output directories at configure time
    if(CMAKE_CONFIGURATION_TYPES)
        # Multi-config generators (Visual Studio)
        foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
            set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
            
            # Create the directory if it doesn't exist
            file(MAKE_DIRECTORY ${OUTPUT_DIR})
            
            # Copy DLL at configure time
            file(COPY ${OPENAL_DLL_FILE} DESTINATION ${OUTPUT_DIR})
            
            message(STATUS "OpenAL: Copied DLL to ${OUTPUT_DIR}")
        endforeach()
    else()
        # Single-config generator (Ninja, Makefiles)
        set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
        
        # Create the directory if it doesn't exist
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        
        # Copy DLL at configure time
        file(COPY ${OPENAL_DLL_FILE} DESTINATION ${OUTPUT_DIR})
        
        message(STATUS "OpenAL: Copied DLL to ${OUTPUT_DIR}")
    endif()
    
    # Store the DLL path for use in other CMakeLists.txt files
    set(OPENAL_DLL_SOURCE ${OPENAL_DLL_FILE} CACHE INTERNAL "Path to OpenAL DLL")
endif()

message(STATUS "OpenAL: Library configured successfully")
message(STATUS "OpenAL: Include directory: ${OPENAL_INCLUDE_DIR}")