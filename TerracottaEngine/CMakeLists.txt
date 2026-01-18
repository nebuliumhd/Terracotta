cmake_minimum_required(VERSION 3.10)

project("TerracottaEngine")

set(ENGINE_INC ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add source to this project's executable.
add_executable(Terracotta ${ENGINE_SRC})

# TODO: Add tests and install targets if needed.
# DON'T NEED TO MANUALLY LIST LIBRARY INCLUDE DIRECTORIES IF THEIR LIBRARY TARGET'S INCLUDES IS PUBLIC
target_include_directories(Terracotta
	PRIVATE 
		${ENGINE_INC} # TerracottaEngine
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/entt/include # Header only library
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/uuid/include
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/json/include
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/fastnoiselite/include
)

# Link libraries here (LINK FROM MOST DEPENDENT TO LEAST DEPENDENT)
target_link_libraries(Terracotta
	PRIVATE
		game # Shared lib
		imgui # Objects
		glfw # Static lib
		glad # Objects
		spdlog # Static lib
		stb # Objects
		OpenAL # OpenAL Soft library
)

# Copy OpenAL DLL to executable directory at build time (Windows only)
if(WIN32 AND OPENAL_DLL_SOURCE)
    add_custom_command(TARGET Terracotta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENAL_DLL_SOURCE}
            $<TARGET_FILE_DIR:Terracotta>
        COMMENT "Copying OpenAL32.dll to executable directory"
        VERBATIM
    )
endif()