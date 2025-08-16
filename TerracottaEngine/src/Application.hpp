#pragma once

#include <vector>
#include <memory>
#include "Window.hpp"
#include "Renderer.hpp"
#include "Subsystem.hpp"
#include "EventSystem.hpp"
#include "InputSystem.hpp"

namespace TerracottaEngine
{
class Application
{
public:
	Application(int windowWidth, int windowHeight);
	~Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void Run();
	void Stop();
	bool IsAppRunning() const { return m_running; }
private:
	void processInput();
	void update(const float deltaTime);
	void render();

	std::unique_ptr<SubsystemManager> m_subsystemManager = nullptr;
	std::unique_ptr<InputSystem> m_inputSystem = nullptr;
	std::unique_ptr<EventSystem> m_eventSystem = nullptr;
	std::unique_ptr<Renderer> m_renderer = nullptr;

	std::unique_ptr<Window> m_window = nullptr;
	bool m_running = true;
	int m_maxFPS, m_maxUPS = 60;
	float m_targetFrameDelay, m_targetUpdateDelay = 1.0f / (float)m_maxUPS;
};
}