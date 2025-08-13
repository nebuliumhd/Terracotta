# CMakeList.txt : CMake project for Terracotta, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.10)

project("TerracottaEngine")

add_subdirectory(vendor/imgui-docking)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)

set(ENGINE_INC ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add source to this project's executable.
add_executable(Terracotta ${ENGINE_SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Terracotta PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_include_directories(Terracotta
	PRIVATE 
		${ENGINE_INC} # TerracottaEngine
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include # GLFW
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include # GLM
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include # Glad

)

# Link libraries here (LINK FROM MOST DEPENDENT TO LEAST DEPENDENT)
target_link_libraries(Terracotta
	PRIVATE
		imgui # Objects
		glfw # Static lib
		glm # Static lib
		glad # Objects
		spdlog # Static lib
)