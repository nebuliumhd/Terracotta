#pragma once

#include <array>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "Window.hpp"
#include "Subsystem.hpp"
#include "EventSystem.hpp"

namespace TerracottaEngine
{
enum class InputState : uint8_t
{
	RELEASED = 0,
	PRESSED = 1,
	REPEATED = 2
};

class InputSystem : public Subsystem
{
public:
	InputSystem(SubsystemManager& manager);
	~InputSystem();

	bool Init() override;
	void OnUpdate(const float deltaTime) override;
	void Shutdown() override;

	void RegisterCallbacks(EventSystem* es);

	bool IsKeyPressed(int key);
	bool IsKeyRepeated(int key);
	bool IsKeyDown(int key);
	bool IsMouseButtonPressed(int mouse);
private:
	std::array<InputState, GLFW_KEY_LAST - GLFW_KEY_SPACE + 1> m_keys;
	std::array<InputState, GLFW_MOUSE_BUTTON_LAST + 1> m_mouse;
};
}