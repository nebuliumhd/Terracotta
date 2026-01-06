#pragma once

#include <vector>
#include <memory>
#include <string>

#include "PlatformDLL.hpp"
#include "EngineAPI.hpp"
#include "Subsystem.hpp"
#include "Layers.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "EventSystem.hpp"
#include "InputSystem.hpp"
#include "AudioSystem.hpp"

namespace TerracottaEngine
{
using GameInitFn = void*(*)(EngineAPI*);
using GameUpdateFn = void(*)(void*, float);
using GameShutdownFn = void(*)(void*);
using GameSerializeStateFn = void*(*)(void*, size_t*);
using GameDeserializeStateFn = void*(*)(EngineAPI*, void*, size_t);

struct GameAPI
{
	GameInitFn Init = nullptr;
	GameUpdateFn Update = nullptr;
	GameShutdownFn Shutdown = nullptr;
	GameSerializeStateFn SerializeState = nullptr;
	GameDeserializeStateFn DeserializeState = nullptr;
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

	// FOR THE ENGINE_API:
	void Log(const char* msg) { SPDLOG_INFO("Hello from {}", msg); }
	bool IsKeyStartPress(int key) { return m_inputSystem->IsKeyStartPress(key); }
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
	std::string m_dllTempPath; // Path to temp copy (for cleanup)
	GameAPI m_gameAPI;
	EngineAPI m_engineAPI;
	void* m_gameInstance = nullptr;
	
	bool loadGameDLL();
	void unloadGameDLL();
	void reloadGameDLL();
	void cleanupOrphanedTempDLLs();

	// Layers
	LayerStack m_layers;
	bool m_running = true;
	int m_maxFPS, m_maxUPS;
	float m_targetFrameDelay, m_targetUpdateDelay;
};
}