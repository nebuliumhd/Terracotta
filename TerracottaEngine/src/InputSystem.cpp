#include "spdlog/spdlog.h"
#include "InputSystem.hpp"

#define KEY_INDEX(Keycode) Keycode - GLFW_KEY_SPACE

namespace TerracottaEngine
{
InputSystem::InputSystem(SubsystemManager& manager) :
	Subsystem(manager)
{
	m_keys.fill(InputState::RELEASED);
	m_mouse.fill(InputState::RELEASED);
}
InputSystem::~InputSystem()
{
	Shutdown();
}

bool InputSystem::Init()
{
	registerCallbacks();
	return true;
}
void InputSystem::OnUpdate(const float deltaTime)
{

}
void InputSystem::Shutdown()
{

}

void InputSystem::registerCallbacks()
{
	// TODO: Move to seperate member functions
	EventSystem* es = m_manager.GetSubsystem<EventSystem>();
	es->AddListener<KeyPressEvent>([this](const KeyPressEvent& e) { m_keys[KEY_INDEX(e.Keycode)] = InputState::PRESSED; });
	es->AddListener<KeyRepeatEvent>([this](const KeyRepeatEvent& e) { m_keys[KEY_INDEX(e.Keycode)] = InputState::REPEATED; });
	es->AddListener<KeyReleaseEvent>([this](const KeyReleaseEvent& e) { m_keys[KEY_INDEX(e.Keycode)] = InputState::RELEASED; });
	es->AddListener<MouseButtonPressEvent>([this](const MouseButtonPressEvent& e) { m_mouse[e.Keycode] = InputState::PRESSED; });
	es->AddListener<MouseButtonReleaseEvent>([this](const MouseButtonReleaseEvent& e) { m_mouse[e.Keycode] = InputState::RELEASED; });
}

bool InputSystem::IsKeyPressed(int key)
{
	return m_keys[KEY_INDEX(key)] == InputState::PRESSED;
}
bool InputSystem::IsKeyRepeated(int key)
{
	return m_keys[KEY_INDEX(key)] == InputState::REPEATED;
}
bool InputSystem::IsKeyDown(int key)
{
	InputState state = m_keys[KEY_INDEX(key)];
	return state == InputState::PRESSED || state == InputState::REPEATED;
}
bool InputSystem::IsMouseButtonPressed(int mouse)
{
	return m_mouse[mouse] == InputState::PRESSED;
}

//void InputSystem::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	int index = key - GLFW_KEY_SPACE;
//	switch (action) {
//	case GLFW_REPEAT:
//		m_keys[index] = InputState::REPEATED;
//		break;
//	case GLFW_PRESS:
//		m_keys[index] = InputState::PRESSED;
//		break;
//	case GLFW_RELEASE:
//	default:
//		m_keys[index] = InputState::RELEASED;
//	}
//}
//void InputSystem::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//{
//	if (action == GLFW_PRESS) {
//		m_mouse[button] = InputState::PRESSED;
//	} else {
//		m_mouse[button] = InputState::RELEASED;
//	}
//}
}