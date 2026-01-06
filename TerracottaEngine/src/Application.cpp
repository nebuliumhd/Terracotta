// Hot reload for Unix operating systems

#include <filesystem>
#include <string>
#include "spdlog/spdlog.h"
#include "Application.hpp"
#include "PlatformDLL.hpp"
#include "EngineAPI.hpp"
#include "InputSystem.hpp"

namespace TerracottaEngine
{
// TODO: Move these down below and use function prototypes
static void Log(EngineHandle inst, const char* msg)
{
	reinterpret_cast<Application*>(inst)->Log(msg);
}
static bool IsKeyStartPress(EngineHandle inst, int key)
{
	return reinterpret_cast<Application*>(inst)->IsKeyStartPress(key);
}

Application::Application(int windowWidth, int windowHeight)
{
	// Logging
	SPDLOG_INFO("Creating application...");

	// Initialize other important systems
	m_window = std::make_unique<Window>(windowWidth, windowHeight);
	m_maxUPS = 60;
	m_targetUpdateDelay = 1.0f / (float)m_maxUPS;
	m_maxFPS = m_window->GetPrimaryMonitorRefreshRate();
	m_targetFrameDelay = 1.0f / (float)m_maxFPS;
	SPDLOG_INFO("m_maxFPS: {} at {}ms", m_maxFPS, m_targetFrameDelay);
	SPDLOG_INFO("m_maxUPS: {} at {}ms", m_maxUPS, m_targetUpdateDelay);

	// TODO: Avoid using the new keyword in the future
	m_subsystemManager = std::make_unique<SubsystemManager>(*this);

	m_eventSystem = std::make_unique<EventSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_eventSystem.get());
	m_eventSystem->LinkToGLFWWindow(m_window->GetGLFWWindow());

	m_inputSystem = std::make_unique<InputSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_inputSystem.get());

	m_audioSystem = std::make_unique<AudioSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_audioSystem.get());
	
	// Load the audio system in the future
	ALuint musicBuffer = m_audioSystem->LoadAudio("../../../../../TerracottaEngine/res/audio.ogg");
	ALuint musicSource = m_audioSystem->CreateAudioSource(musicBuffer);
	SPDLOG_INFO("Buffer: {}, Source: {}", musicBuffer, musicSource);
	m_audioSystem->PlayAudio(musicSource);

	m_renderer = std::make_unique<Renderer>(*m_subsystemManager, *m_window);
	m_subsystemManager->RegisterSubsystem(m_renderer.get());

	m_layers.PushLayer(new DearImGuiLayer(m_window->GetGLFWWindow(), "Main DearImGui Layer"));

	// Game initialization
	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to load the game's shared library!");
	}

	m_engineAPI = EngineAPI{
		this,
		&TerracottaEngine::Log,
		&TerracottaEngine::IsKeyStartPress
	};
	m_gameInstance = m_gameAPI.Init(&m_engineAPI);
	SPDLOG_INFO("Loaded the game's shared library.");

	SPDLOG_INFO("Finished creating application.");
}
Application::~Application()
{
	SPDLOG_INFO("Application destructor called - cleaning up...");
	// Ensure that we unload the game DLL and clear temp files
	unloadGameDLL();
	// Clean up any orphaned temp DLLs from previous runs
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

	// WARNING: Do NOT have code in any classes that rely on process input to move!
	glfwPollEvents(); // Keep this tied to update for now

	// Should update if enough time has elapsed (default = ~0.0167ms for 60 UPS)
	// USE WHILE LOOP!!!
	while (updateAcc >= m_targetUpdateDelay) {
		update(deltaTime); // Update engine first before the game

		if (m_gameInstance)
			m_gameAPI.Update(m_gameInstance, deltaTime);
		
		// TEMP: Make it so that at the end of frame we update this
		m_inputSystem->OnUpdateEnd();
		
		updateAcc -= m_targetUpdateDelay;
	}

	// Should render if enough time has elapsed (default = same as UPS)
	// USE IF STATEMENT!!!
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
	if (m_inputSystem->IsKeyStartPress(GLFW_KEY_F5)) {
		SPDLOG_INFO("Hot reloading game DLL...");
		reloadGameDLL();
	}
	if (m_inputSystem->IsKeyStartPress(GLFW_KEY_Q)) {
		SPDLOG_INFO("I have inputs registered!");
	}
	if (m_inputSystem->IsKeyStartPress(GLFW_KEY_E)) {
		SPDLOG_WARN("Unregistering ALL input system callbacks...");
		m_inputSystem->UnregisterCallbacks();
	}

	const std::vector<Subsystem*>& subsystems = m_subsystemManager->GetAllSubsystems();
	for (Subsystem* sub : subsystems) {
		sub->OnUpdate(deltaTime);
	}
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

	std::filesystem::path exePath = std::filesystem::current_path();
	std::filesystem::path dllPath = exePath / dllName;
	
	SPDLOG_INFO("Loading game DLL from: {}", dllPath.string());

	DLLLoadResult result = LoadDLLForHotReload(dllName);
	m_dllHandle = result.Handle;
	m_dllTempPath = result.TempPath;
	if (!m_dllHandle) {
		SPDLOG_ERROR("Failed to get the handle for the game DLL! Make sure it's called game.dll/libgame.dylib/libgame.so");
		return false;
	}

	// Get function pointers from the loaded DLL
	m_gameAPI.Init = (GameInitFn)GetDLLSymbol(m_dllHandle, "GameInit");
	m_gameAPI.Update = (GameUpdateFn)GetDLLSymbol(m_dllHandle, "GameUpdate");
	m_gameAPI.Shutdown = (GameShutdownFn)GetDLLSymbol(m_dllHandle, "GameShutdown");
	m_gameAPI.SerializeState = (GameSerializeStateFn)GetDLLSymbol(m_dllHandle, "GameSerializeState");
	m_gameAPI.DeserializeState = (GameDeserializeStateFn)GetDLLSymbol(m_dllHandle, "GameDeserializeState");

	if (!m_gameAPI.Init || !m_gameAPI.Update || !m_gameAPI.Shutdown) {
		SPDLOG_ERROR("Failed to load one or more required functions from the game DLL!");
		UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
		m_dllHandle = nullptr;
		m_dllTempPath.clear();
		return false;
	}

	// SerializeState and DeserializeState are optional
	if (!m_gameAPI.SerializeState || !m_gameAPI.DeserializeState) {
		SPDLOG_WARN("Serialization functions not found - hot reload will reset game state");
	}

	return true;
}
void Application::unloadGameDLL()
{
	if (!m_dllHandle)
		return;

	if (m_gameAPI.Shutdown && m_gameInstance) {
		m_gameAPI.Shutdown(m_gameInstance);
		m_gameInstance = nullptr;
	}

	UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
	m_dllHandle = nullptr;
	m_dllTempPath.clear();

	m_gameAPI.Init = nullptr;
	m_gameAPI.Update = nullptr;
	m_gameAPI.Shutdown = nullptr;
	m_gameAPI.SerializeState = nullptr;
	m_gameAPI.DeserializeState = nullptr;

	SPDLOG_INFO("Unloaded game DLL.");
}
void Application::reloadGameDLL()
{
	// Step 1: Serialize current state
	void* serializedState = nullptr;
	size_t stateSize = 0;
	if (m_gameAPI.SerializeState && m_gameInstance) {
		serializedState = m_gameAPI.SerializeState(m_gameInstance, &stateSize);
	}

	// Step 2: Shutdown and unload
	if (m_gameAPI.Shutdown && m_gameInstance) {
		m_gameAPI.Shutdown(m_gameInstance);
		m_gameInstance = nullptr;
	}
	UnloadDLLAndCleanup(m_dllHandle, m_dllTempPath);
	m_dllHandle = nullptr;
	m_dllTempPath.clear();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Step 3: Load new DLL
	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to reload the DLL!");
		if (serializedState) free(serializedState);
		return;
	}

	// Step 4: Restore state or create fresh
	if (serializedState && m_gameAPI.DeserializeState) {
		m_gameInstance = m_gameAPI.DeserializeState(&m_engineAPI, serializedState, stateSize);
		free(serializedState);
	} else {
		m_gameInstance = m_gameAPI.Init(&m_engineAPI);
	}
	
	SPDLOG_INFO("Successfully hot-reloaded the game DLL!");
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
}