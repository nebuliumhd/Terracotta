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
static void glfwWindowPosCallback(GLFWwindow* window, int xpos, int ypos);
static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
static void glfwWindowCloseCallback(GLFWwindow* window);
static void glfwWindowFocusCallback(GLFWwindow* window, int focused);
static void glfwWindowIconifyCallback(GLFWwindow* window, int iconified);
static void glfwWindowMaximizeCallback(GLFWwindow* window, int maximized);
static void glfwWindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale);

void EventSystem::LinkToGLFWWindow(GLFWwindow* glfwWindow)
{
	if (!glfwWindow) {
		SPDLOG_ERROR("Invalid GLFWwindow* passed!");
		return;
	}

	// All callbacks
	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
	glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
	glfwSetWindowPosCallback(glfwWindow, glfwWindowPosCallback);
	glfwSetWindowSizeCallback(glfwWindow, glfwWindowSizeCallback);
	glfwSetWindowCloseCallback(glfwWindow, glfwWindowCloseCallback);
	glfwSetWindowFocusCallback(glfwWindow, glfwWindowFocusCallback);
	glfwSetWindowIconifyCallback(glfwWindow, glfwWindowIconifyCallback);
	glfwSetWindowMaximizeCallback(glfwWindow, glfwWindowMaximizeCallback);
	glfwSetWindowContentScaleCallback(glfwWindow, glfwWindowContentScaleCallback);

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

static void glfwWindowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowPositionEvent event;
	event.X = xpos;
	event.Y = ypos;
	es->DispatchEvent(event);
}

static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowSizeEvent event;
	event.Width = width;
	event.Height = height;
	es->DispatchEvent(event);
}

static void glfwWindowCloseCallback(GLFWwindow* window)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowCloseEvent event;
	es->DispatchEvent(event);
}

static void glfwWindowFocusCallback(GLFWwindow* window, int focused)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowFocusEvent event;
	if (focused == GLFW_TRUE) {
		event.Focused = true;
	} else {
		event.Focused = false;
	}
	es->DispatchEvent(event);
}

static void glfwWindowIconifyCallback(GLFWwindow* window, int iconified)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowIconifyEvent event;
	if (iconified == GLFW_TRUE) {
		event.Iconified = true;
	} else {
		event.Iconified = false;
	}
	es->DispatchEvent(event);
}

static void glfwWindowMaximizeCallback(GLFWwindow* window, int maximized)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowMaximizeEvent event;
	if (maximized == GLFW_TRUE) {
		event.Maximized = true;
	} else {
		event.Maximized = false;
	}
	es->DispatchEvent(event);
}

static void glfwWindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale)
{
	EventSystem* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	assert(es && "EventSystem is nullptr!");

	WindowContentScaleEvent event;
	event.XScale = xscale;
	event.YScale = yscale;
	es->DispatchEvent(event);
}
} // namespace TerracottaEngine