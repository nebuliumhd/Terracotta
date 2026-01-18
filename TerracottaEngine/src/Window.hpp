#pragma once

#include "glm/glm.hpp"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace TerracottaEngine
{
class Window
{
public:
	Window(int windowWidth, int windowHeight);
	~Window();

	GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }
	int GetPrimaryMonitorRefreshRate() const;
	glm::ivec2 GetWindowSize() const { return {m_windowWidth, m_windowHeight}; }
private:
	static void glfwErrorCallback(int error, const char* description);

	GLFWwindow* m_glfwWindow = nullptr;
	GLFWmonitor* m_primaryMonitor = nullptr;
	int m_windowWidth, m_windowHeight;
};
} // namespace TerracottaEngine