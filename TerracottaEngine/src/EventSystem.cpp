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
	return true;
}
void EventSystem::OnUpdate(const float deltaTime)
{

}
void EventSystem::Shutdown()
{

}

// TODO/WARNING: Might need to move the template functions into the header file!
static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void EventSystem::LinkToGLFWWindow(GLFWwindow* glfwWindow)
{
	if (!glfwWindow) {
		SPDLOG_ERROR("Invalid GLFWwindow* passed!");
		return;
	}

	// All callbacks
	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);

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
}