cmake_minimum_required(VERSION 3.10)

project("TerracottaGame")

add_library(game SHARED
	
)

target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})