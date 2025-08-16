#pragma once

#include <vector>
#include <memory>
#include "Subsystem.hpp"
#include "Layers.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
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

	// Subsystems
	std::unique_ptr<SubsystemManager> m_subsystemManager = nullptr;
	std::unique_ptr<InputSystem> m_inputSystem = nullptr;
	std::unique_ptr<EventSystem> m_eventSystem = nullptr;
	std::unique_ptr<Renderer> m_renderer = nullptr;

	std::unique_ptr<Window> m_window = nullptr;

	// Layers
	LayerStack m_layers;
	bool m_running = true;
	int m_maxFPS, m_maxUPS;
	float m_targetFrameDelay, m_targetUpdateDelay;
};
}