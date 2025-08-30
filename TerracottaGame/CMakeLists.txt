cmake_minimum_required(VERSION 3.10)

project("TerracottaGame")

set(GAME_INC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ENGINE_INC ${CMAKE_SOURCE_DIR}/TerracottaEngine/src)
file(GLOB_RECURSE GAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(game SHARED ${GAME_SRC})

target_compile_options(game PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# TODO: Hide headers from game engine (only expose public API)
target_include_directories(game PUBLIC ${GAME_INC})

# Only header files to talk to libraries
target_include_directories(game
	PRIVATE 
		${ENGINE_INC} # TerracottaEngine
		${CMAKE_SOURCE_DIR}/TerracottaEngine/vendor/spdlog/include
)