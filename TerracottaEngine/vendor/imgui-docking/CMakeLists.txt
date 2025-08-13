cmake_minimum_required(VERSION 3.10)

project(IMGUI)

# Gather include folder for ImGui and its dependencies
set(GLAD_INC ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include)
set(STB_INC ${CMAKE_CURRENT_SOURCE_DIR}/../stb/include)
set(GLFW_INC ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include)
set(IMGUI_INC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Gather source files for ImGui
file(GLOB IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Create objects so that ImGui can be used in other CMake files
add_library(imgui OBJECT ${IMGUI_SRC})

# Add the include directory so that other targets can use ImGui publicly
target_include_directories(imgui
    PUBLIC
        ${IMGUI_INC}
        ${GLFW_INC}
        ${STB_INC}
        ${GLAD_INC}
)