cmake_minimum_required(VERSION 3.10)

project("TerracottaGame")

set(GAME_INC ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE GAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(game SHARED ${GAME_SRC})

# TODO: Hide headers from game engine (only expose public API)
target_include_directories(game PUBLIC ${GAME_INC})