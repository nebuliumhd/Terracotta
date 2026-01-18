#include "spdlog/spdlog.h"
#include "InputSystem.hpp"

#define KEY_INDEX(Keycode) Keycode - GLFW_KEY_SPACE

namespace TerracottaEngine
{
InputSystem::InputSystem(SubsystemManager& manager) :
	Subsystem(manager)
{
	m_currKeyInputs.fill(InputState::RELEASED);
	m_prevKeyInputs = m_currKeyInputs;
	m_currMouseInputs.fill(InputState::RELEASED);
	m_prevMouseInputs = m_currMouseInputs;
}
InputSystem::~InputSystem()
{
	
}

bool InputSystem::Init()
{
	registerCallbacks();
	SPDLOG_INFO("EventSystem initialization complete.");
	return true;
}
void InputSystem::OnUpdate(const float deltaTime)
{

}
void InputSystem::Shutdown()
{
	SPDLOG_INFO("InputSystem shutdown complete.");
}

void InputSystem::OnUpdateEnd()
{
	m_prevKeyInputs = m_currKeyInputs;
	m_prevMouseInputs = m_currMouseInputs;
}

void InputSystem::registerCallbacks()
{
	// TODO: Move to seperate member functions
	EventSystem* es = m_manager.GetSubsystem<EventSystem>();
	es->AddListener<KeyPressEvent>([this](const KeyPressEvent& e) { m_currKeyInputs[KEY_INDEX(e.Keycode)] = InputState::PRESSED; });
	es->AddListener<KeyRepeatEvent>([this](const KeyRepeatEvent& e) { m_currKeyInputs[KEY_INDEX(e.Keycode)] = InputState::REPEATED; });
	es->AddListener<KeyReleaseEvent>([this](const KeyReleaseEvent& e) { m_currKeyInputs[KEY_INDEX(e.Keycode)] = InputState::RELEASED; });
	es->AddListener<MouseButtonPressEvent>([this](const MouseButtonPressEvent& e) { m_currMouseInputs[e.Keycode] = InputState::PRESSED; });
	es->AddListener<MouseButtonReleaseEvent>([this](const MouseButtonReleaseEvent& e) { m_currMouseInputs[e.Keycode] = InputState::RELEASED; });
}

bool InputSystem::IsKeyStartPress(int key)
{
	int keyIndex = KEY_INDEX(key);
	return m_prevKeyInputs[keyIndex] != InputState::PRESSED && m_currKeyInputs[keyIndex] == InputState::PRESSED;
}
bool InputSystem::IsKeyEndPress(int key)
{
	int keyIndex = KEY_INDEX(key);
	return m_prevKeyInputs[keyIndex] != InputState::RELEASED && m_currKeyInputs[keyIndex] == InputState::RELEASED;
}
bool InputSystem::IsKeyRepeat(int key)
{
	return m_currKeyInputs[KEY_INDEX(key)] == InputState::REPEATED;
}
bool InputSystem::IsKeyDown(int key)
{
	InputState state = m_currKeyInputs[KEY_INDEX(key)];
	return state == InputState::PRESSED || state == InputState::REPEATED;
}
bool InputSystem::IsMouseButtonStartPress(int mouse)
{
	return m_prevMouseInputs[mouse] != InputState::PRESSED && m_currMouseInputs[mouse] == InputState::PRESSED;
}
bool InputSystem::IsMouseButtonEndPress(int mouse)
{
	return m_prevMouseInputs[mouse] != InputState::RELEASED && m_currMouseInputs[mouse] == InputState::RELEASED;
}
bool InputSystem::IsMouseButtonDown(int mouse)
{
	return m_currMouseInputs[mouse] == InputState::PRESSED || m_currMouseInputs[mouse] == InputState::REPEATED;
}
} // namespace TerracottaEngine