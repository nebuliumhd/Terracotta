cmake_minimum_required(VERSION 3.10)

project(GLAD)

set(GLAD_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(GLOB GLAD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
    
# Create static library (.obj/.o)
add_library(glad OBJECT ${GLAD_SRC})

# Add the include directory so that other targets can use GLAD publicly
target_include_directories(glad PUBLIC ${GLAD_INC})