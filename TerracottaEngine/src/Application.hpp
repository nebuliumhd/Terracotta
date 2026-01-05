#pragma once

#include <vector>
#include <memory>

#include "PlatformDLL.hpp"
#include "Subsystem.hpp"
#include "Layers.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "EventSystem.hpp"
#include "InputSystem.hpp"
#include "AudioSystem.hpp"
#include "EngineAPI.hpp"

namespace TerracottaEngine
{
using GameInitFn = void*(*)(EngineAPI*);
using GameUpdateFn = void(*)(void*, float);
using GameShutdownFn = void(*)(void*);

struct GameAPI
{
	GameInitFn Init = nullptr;
	GameUpdateFn Update = nullptr;
	GameShutdownFn Shutdown = nullptr;
};

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

	void Log(const char* msg) { SPDLOG_INFO("Hello from {}", msg); }
private:
	void update(const float deltaTime);
	void render();

	// Subsystems
	std::unique_ptr<SubsystemManager> m_subsystemManager = nullptr;
	std::unique_ptr<InputSystem> m_inputSystem = nullptr;
	std::unique_ptr<EventSystem> m_eventSystem = nullptr;
	std::unique_ptr<AudioSystem> m_audioSystem = nullptr;
	std::unique_ptr<Renderer> m_renderer = nullptr;
	std::unique_ptr<Window> m_window = nullptr;

	// Game
	DLL_HANDLE m_dllHandle = nullptr;
	GameAPI m_gameAPI;
	EngineAPI m_engineAPI;
	void* m_gameInstance = nullptr;
	// Make sure to manually call the m_gameAPI.Init()
	bool loadGameDLL();
	void unloadGameDLL();
	void reloadGameDLL();

	// Layers
	LayerStack m_layers;
	bool m_running = true;
	int m_maxFPS, m_maxUPS;
	float m_targetFrameDelay, m_targetUpdateDelay;
};
}