cmake_minimum_required(VERSION 3.10)

project("TerracottaGame")

set(GAME_INC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ENGINE_INC ${CMAKE_SOURCE_DIR}/TerracottaEngine/src)
file(GLOB_RECURSE GAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(game SHARED ${GAME_SRC})

target_compile_options(game PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Windows-specific hot-reload optimizations (Debug only)
if(WIN32)
    # Disable incremental linking in Debug to reduce file locks
    target_link_options(game PRIVATE 
        $<$<CONFIG:Debug>:/INCREMENTAL:NO>
        /OPT:REF
        /OPT:ICF
    )
    
    # Move PDB to separate directory to avoid locks
    set_target_properties(game PROPERTIES
        COMPILE_PDB_NAME "game"
    )
endif()

# Define ENABLE_HOT_RELOAD only in Debug builds
target_compile_definitions(game PRIVATE 
    $<$<CONFIG:Debug>:ENABLE_HOT_RELOAD>
)

# TODO: Hide headers from game engine (only expose public API)
target_include_directories(game PUBLIC ${GAME_INC})

# Only header files to talk to libraries
target_include_directories(game
	PRIVATE 
		${ENGINE_INC} # TerracottaEngine
		${CMAKE_SOURCE_DIR}/TerracottaEngine/vendor/spdlog/include
)