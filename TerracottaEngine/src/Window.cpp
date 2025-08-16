#include "spdlog/spdlog.h"
#include "glad/glad.h"
#include "Window.hpp"

namespace TerracottaEngine
{
Window::Window(int windowWidth, int windowHeight) :
	m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
	SPDLOG_INFO("Intiailizing GLFW with a {}x{} window...", windowWidth, windowHeight);

	if (!glfwInit()) {
		SPDLOG_ERROR("Failed to initialize GLFW!");
		return;
	}
	glfwSetErrorCallback(Window::glfwErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Sample Window", nullptr, nullptr);
	if (!m_glfwWindow) {
		SPDLOG_ERROR("Failed to create a GLFW window!");
		return;
	}
	m_primaryMonitor = glfwGetPrimaryMonitor();

	glfwMakeContextCurrent(m_glfwWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		SPDLOG_ERROR("Failed to get GLFW's process address!");
		return;
	}
	glfwSwapInterval(1); // VSYNC = 1

	// OpenGL settings and callbacks later

	SPDLOG_INFO("Finished initializing GLFW.");
}
Window::~Window()
{
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}

int Window::GetPrimaryMonitorRefreshRate() const
{
	const GLFWvidmode* currVidMode = glfwGetVideoMode(m_primaryMonitor);
	return currVidMode->refreshRate;
}

void Window::glfwErrorCallback(int error, const char* description)
{
	SPDLOG_ERROR("GLFW Error {}: {}", error, description);
}
}