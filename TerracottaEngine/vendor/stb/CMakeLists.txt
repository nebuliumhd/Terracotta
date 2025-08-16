cmake_minimum_required(VERSION 3.10)

project(STB)

set(STB_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(GLOB STB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
    
# Create objects so that stb can be used in other CMake files
add_library(stb OBJECT ${STB_SRC})

# Add the include directory so that other targets can use GLAD publicly
target_include_directories(stb PUBLIC ${STB_INC})