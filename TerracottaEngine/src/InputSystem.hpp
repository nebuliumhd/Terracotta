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

	void OnUpdateEnd();

	// Use GLFW keycodes for now
	bool IsKeyStartPress(int key);
	bool IsKeyEndPress(int key);
	bool IsKeyRepeat(int key);
	bool IsKeyDown(int key);
	bool IsMouseButtonStartPress(int mouse);
	bool IsMouseButtonEndPress(int mouse);
	bool IsMouseButtonDown(int mouse);
private:
	// NOTE MEDIA PLAYBACK KEYS AND OTHER STUFF WILL GO OUT OF BOUNDS!
	std::array<InputState, GLFW_KEY_LAST - GLFW_KEY_SPACE + 1> m_prevKeyInputs;
	std::array<InputState, GLFW_KEY_LAST - GLFW_KEY_SPACE + 1> m_currKeyInputs;
	std::array<InputState, GLFW_MOUSE_BUTTON_LAST + 1> m_prevMouseInputs;
	std::array<InputState, GLFW_MOUSE_BUTTON_LAST + 1> m_currMouseInputs;
	
	// For debugging purposes!
	uint64_t m_thatOneCallbackID = 0;

	void registerCallbacks();
};
} // namespace TerracottaEngine