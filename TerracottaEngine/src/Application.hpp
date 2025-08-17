#pragma once

#include <vector>
#include <memory>
#include "Subsystem.hpp"
#include "Layers.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "EventSystem.hpp"
#include "InputSystem.hpp"
#include "AudioSystem.hpp"

// From the game itself
#include "Foobar.hpp"

using GameInitFn = void(*)(GameState*);
using GameUpdateFn = void(*)(GameState*, float);
using GameShutdownFn = void(*)(GameState*);

struct GameAPI
{
	GameInitFn Init = nullptr;
	GameUpdateFn Update = nullptr;
	GameShutdownFn Shutdown = nullptr;
};

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
	void update(const float deltaTime);
	void render();

	// TODO: Move hot reload functions here

	// Subsystems
	std::unique_ptr<SubsystemManager> m_subsystemManager = nullptr;
	std::unique_ptr<InputSystem> m_inputSystem = nullptr;
	std::unique_ptr<EventSystem> m_eventSystem = nullptr;
	std::unique_ptr<AudioSystem> m_audioSystem = nullptr;
	std::unique_ptr<Renderer> m_renderer = nullptr;

	std::unique_ptr<Window> m_window = nullptr;

	// Layers
	LayerStack m_layers;
	bool m_running = true;
	int m_maxFPS, m_maxUPS;
	float m_targetFrameDelay, m_targetUpdateDelay;
};
}