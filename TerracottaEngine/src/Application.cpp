// Hot reload for Unix operating systems

#include <filesystem>
#include <string>
#include "spdlog/spdlog.h"
#include "EngineAPI.h"
#include "Application.hpp"
#include "PlatformDLL.hpp"
#include "InputSystem.hpp"

namespace TerracottaEngine
{
Application::Application(int windowWidth, int windowHeight)
{
	SPDLOG_INFO("Creating application...");

	// Initialize window and subsystems
	m_window = std::make_unique<Window>(windowWidth, windowHeight);
	m_maxUPS = 60;
	m_targetUpdateDelay = 1.0f / (float)m_maxUPS;
	m_maxFPS = m_window->GetPrimaryMonitorRefreshRate();
	m_targetFrameDelay = 1.0f / (float)m_maxFPS;

	m_subsystemManager = std::make_unique<SubsystemManager>();
	SubsystemManager& managerRef = *m_subsystemManager;
	m_eventSystem = m_subsystemManager->RegisterSubsystem<EventSystem>(managerRef);
	m_eventSystem->LinkToGLFWWindow(m_window->GetGLFWWindow());
	m_inputSystem = m_subsystemManager->RegisterSubsystem<InputSystem>(managerRef);
	m_audioSystem = m_subsystemManager->RegisterSubsystem<AudioSystem>(managerRef);
	m_randomGenerator = m_subsystemManager->RegisterSubsystem<RandomGenerator>(managerRef);
	m_renderer = m_subsystemManager->RegisterSubsystem<Renderer>(managerRef, *m_window);
	m_layers.PushLayer(new DearImGuiLayer(m_window->GetGLFWWindow(), "Main DearImGui Layer"));

	// Create Engine API struct with function pointers
	m_engineAPI = EngineAPI_Create(this);

	// Load and initialize game DLL
	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to load the game's shared library!");
	}

	SPDLOG_INFO("Finished creating application.");
}
Application::~Application()
{
	SPDLOG_INFO("Application destructor called - cleaning up...");
	unloadGameDLL();
	cleanupOrphanedTempDLLs();
	SPDLOG_INFO("Application cleanup complete.");
}

void Application::Run()
{
	static float prevTime = (float)glfwGetTime(), updateAcc = 0.0f, renderAcc = 0.0f;

	float currTime = (float)glfwGetTime();
	const float deltaTime = currTime - prevTime;
	prevTime = currTime;
	updateAcc += deltaTime;
	renderAcc += deltaTime;

	glfwPollEvents();

	while (updateAcc >= m_targetUpdateDelay) {
		update(m_targetUpdateDelay);
		updateAcc -= m_targetUpdateDelay;
	}

	while (renderAcc >= m_targetFrameDelay) {
		render();
		renderAcc -= m_targetFrameDelay;
	}

	if (glfwWindowShouldClose(m_window->GetGLFWWindow())) {
		Stop();
	}
}

void Application::Stop()
{
	m_running = false;
	unloadGameDLL();
	// Most systems are deinitialized with ~Application.
}

void Application::update(const float deltaTime)
{
	// Hot reload check
	if (m_inputSystem->IsKeyStartPress(GLFW_KEY_F5)) {
		SPDLOG_INFO("Hot reloading game DLL...");
		reloadGameDLL();
	}

	// Audio test keys
	if (m_inputSystem->IsKeyDown(GLFW_KEY_0)) {
		m_audioSystem->PlayAudio("../../../../../TerracottaEngine/res/bass.ogg");
	} else if (m_inputSystem->IsKeyDown(GLFW_KEY_1)) {
		m_audioSystem->PlayAudio("../../../../../TerracottaEngine/res/drumloop.ogg");
	} else if (m_inputSystem->IsKeyDown(GLFW_KEY_2)) {
		m_audioSystem->PlayAudio("../../../../../TerracottaEngine/res/hit.ogg");
	} else if (m_inputSystem->IsKeyDown(GLFW_KEY_3)) {
		m_audioSystem->PlayAudio("../../../../../TerracottaEngine/res/boing.ogg");
	}

	// Update engine subsystems FIRST
	m_subsystemManager->Update(deltaTime);

	if (m_gameInstance && m_gameUpdateFn) {
		m_gameUpdateFn(m_gameInstance, deltaTime);
	}

	// End of frame cleanup
	m_inputSystem->OnUpdateEnd();
}

void Application::render()
{
	// Buffer clears in main renderer
	m_renderer->OnRender();

	for (Layer* layer : m_layers) {
		layer->OnRender();
		layer->OnImGuiRender();
	}

	glfwSwapBuffers(m_window->GetGLFWWindow());
}

bool Application::loadGameDLL()
{
#ifdef _WIN32
	const char* dllName = "game.dll";
#elif defined(__APPLE__)
	const char* dllName = "libgame.dylib";
#elif defined(__linux__)
	const char* dllName = "libgame.so";
#endif

	SPDLOG_INFO("Loading game DLL from: {}", dllName);

	DLLLoadResult result = LoadDLLForHotReload(dllName);
	m_dllHandle = result.Handle;
	m_dllTempPath = result.TempPath;

	if (!m_dllHandle) {
		SPDLOG_ERROR("Failed to get the handle for the game DLL!");
		return false;
	}

	// Get function pointers
	m_gameInitFn = (GameInitFn)GetDLLSymbol(m_dllHandle, "GameInit");
	m_gameUpdateFn = (GameUpdateFn)GetDLLSymbol(m_dllHandle, "GameUpdate");
	m_gameShutdownFn = (GameShutdownFn)GetDLLSymbol(m_dllHandle, "GameShutdown");
	m_gameSerializeFn = (GameSerializeStateFn)GetDLLSymbol(m_dllHandle, "GameSerializeState");
	m_gameDeserializeFn = (GameDeserializeStateFn)GetDLLSymbol(m_dllHandle, "GameDeserializeState");

	if (!m_gameInitFn || !m_gameUpdateFn || !m_gameShutdownFn) {
		SPDLOG_ERROR("Failed to load required game functions!");
		UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
		m_dllHandle = nullptr;
		return false;
	}

	if (!m_gameSerializeFn || !m_gameDeserializeFn) {
		SPDLOG_WARN("Serialization functions not found - hot reload will reset game state");
	}

	// Initialize game
	m_gameInstance = m_gameInitFn(&m_engineAPI);
	SPDLOG_INFO("Game DLL loaded and initialized.");

	return true;
}

void Application::unloadGameDLL()
{
	if (!m_dllHandle)
		return;

	if (m_gameShutdownFn && m_gameInstance) {
		m_gameShutdownFn(m_gameInstance);
		m_gameInstance = nullptr;
	}

	UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
	m_dllHandle = nullptr;
	m_dllTempPath.clear();

	m_gameInitFn = nullptr;
	m_gameUpdateFn = nullptr;
	m_gameShutdownFn = nullptr;
	m_gameSerializeFn = nullptr;
	m_gameDeserializeFn = nullptr;

	SPDLOG_INFO("Unloaded game DLL.");
}

void Application::reloadGameDLL()
{
	// Serialize current state
	void* serializedState = nullptr;
	size_t stateSize = 0;
	if (m_gameSerializeFn && m_gameInstance) {
		serializedState = m_gameSerializeFn(m_gameInstance, &stateSize);
	}

	// Shutdown and unload
	if (m_gameShutdownFn && m_gameInstance) {
		m_gameShutdownFn(m_gameInstance);
		m_gameInstance = nullptr;
	}
	UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
	m_dllHandle = nullptr;
	m_dllTempPath.clear();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Load new DLL
	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to reload the DLL!");
		if (serializedState)
			free(serializedState);
		return;
	}

	// Restore state if available
	if (serializedState && m_gameDeserializeFn) {
		m_gameInstance = m_gameDeserializeFn(&m_engineAPI, serializedState, stateSize);
		free(serializedState);
		SPDLOG_INFO("Successfully hot-reloaded with state preservation!");
	} else {
		SPDLOG_INFO("Successfully hot-reloaded (state reset)!");
	}
}

void Application::cleanupOrphanedTempDLLs()
{
#ifdef _WIN32
	const char* dllPattern = "game_*.dll";
#elif defined(__APPLE__)
	const char* dllPattern = "libgame_*.dylib";
#elif defined(__linux__)
	const char* dllPattern = "libgame_*.so";
#endif

	try {
		std::filesystem::path currentDir = std::filesystem::current_path();
		
		for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
			if (!entry.is_regular_file())
				continue;
			
			std::string filename = entry.path().filename().string();
			
			// Check if it matches the temp DLL pattern
#ifdef _WIN32
			bool isTemp = filename.find("game_") == 0 && filename.ends_with(".dll");
#elif defined(__APPLE__)
			bool isTemp = filename.find("libgame_") == 0 && filename.ends_with(".dylib");
#elif defined(__linux__)
			bool isTemp = filename.find("libgame_") == 0 && filename.ends_with(".so");
#endif
			
			if (isTemp) {
				try {
					std::filesystem::remove(entry.path());
					SPDLOG_INFO("Cleaned up orphaned temp DLL: {}", filename);
				} catch (const std::exception& e) {
					SPDLOG_WARN("Could not remove orphaned temp DLL {}: {}", filename, e.what());
				}
			}
		}
	} catch (const std::exception& e) {
		SPDLOG_WARN("Error while cleaning up orphaned temp DLLs: {}", e.what());
	}
}
} // namespace TerracottaEngine