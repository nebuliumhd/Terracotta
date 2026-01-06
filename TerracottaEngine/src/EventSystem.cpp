#include <cassert>
#include <type_traits>
#include "spdlog/spdlog.h"
#include "EventSystem.hpp"

namespace TerracottaEngine
{
EventSystem::EventSystem(SubsystemManager& manager) :
	Subsystem(manager)
{
	
}
EventSystem::~EventSystem()
{
	
}
bool EventSystem::Init()
{
	SPDLOG_INFO("EventSystem initialization complete.");
	return true;
}
void EventSystem::OnUpdate(const float deltaTime)
{
	
}
void EventSystem::Shutdown()
{
	// TODO: Perhaps remove the GLFW bindings?
	SPDLOG_INFO("EventSystem shutdown complete.");
}

// TODO/WARNING: Might need to move the template functions into the header file!
static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void EventSystem::LinkToGLFWWindow(GLFWwindow* glfwWindow)
{
	if (!glfwWindow) {
		SPDLOG_ERROR("Invalid GLFWwindow* passed!");
		return;
	}

	// All callbacks
	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
	glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);

	// Store the EventSystem in the GLFWwindow instance
	glfwSetWindowUserPointer(glfwWindow, this);
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	switch (action) {
	case GLFW_REPEAT:
		KeyRepeatEvent repeat;
		repeat.Keycode = key;
		repeat.Mods = mods;
		es->DispatchEvent(repeat);
		break;
	case GLFW_PRESS:
		KeyPressEvent press;
		press.Keycode = key;
		press.Mods = mods;
		es->DispatchEvent(press);
		break;
	case GLFW_RELEASE:
	default:
		KeyReleaseEvent release;
		release.Keycode = key;
		release.Mods = mods;
		es->DispatchEvent(release);
	}
}

static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	switch (action) {
	case GLFW_PRESS:
		MouseButtonPressEvent press;
		press.Keycode = button;
		press.Mods = mods;
		es->DispatchEvent(press);
		break;
	case GLFW_RELEASE:
	default:
		MouseButtonReleaseEvent release;
		release.Keycode = button;
		release.Mods = mods;
		es->DispatchEvent(release);
	}
}

}