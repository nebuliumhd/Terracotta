#pragma once

#include <vector>
#include <memory>
#include <string>

#include "EngineAPI.h"
#include "PlatformDLL.hpp"
#include "Subsystem.hpp"
#include "Layers.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "EventSystem.hpp"
#include "InputSystem.hpp"
#include "AudioSystem.hpp"
#include "RandomGenerator.hpp"

namespace TerracottaEngine
{
using GameInstance = void*;
using GameInitFn = void*(*)(EngineAPI*);
using GameUpdateFn = void(*)(void*, const float);
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

	Renderer* GetRenderer() { return m_renderer; }
	InputSystem* GetInputSystem() { return m_inputSystem; }
	RandomGenerator* GetRandomGenerator() { return m_randomGenerator; }
private:
	void update(const float deltaTime);
	void render();

	// Subsystems
	std::unique_ptr<SubsystemManager> m_subsystemManager = nullptr;
	InputSystem* m_inputSystem = nullptr;
	EventSystem* m_eventSystem = nullptr;
	AudioSystem* m_audioSystem = nullptr;
	Renderer* m_renderer = nullptr;
	RandomGenerator* m_randomGenerator = nullptr;

	// Other systems
	std::unique_ptr<Window> m_window = nullptr;

	GameAPI m_gameAPI;
	EngineAPI m_engineAPI;
	GameInstance m_gameInstance = nullptr;
	GameInitFn m_gameInitFn = nullptr;
	GameUpdateFn m_gameUpdateFn = nullptr;
	GameShutdownFn m_gameShutdownFn = nullptr;
	GameSerializeStateFn m_gameSerializeFn = nullptr;
	GameDeserializeStateFn m_gameDeserializeFn = nullptr;

	// Game
	DLL_HANDLE m_dllHandle = nullptr;
	std::string m_dllTempPath; // Path to temp copy (for cleanup)
	
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
} // namespace TerracottaEngine