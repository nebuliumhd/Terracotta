// Hot reload preprocessors
#ifdef _WIN32
// Windows
#include <Windows.h>
#define DLL_HANDLE HMODULE
#define LoadGameDLL(path) LoadLibraryA(path)
#define GetGameProcAddress(dll, fn) GetProcAddress(dll, fn)
#define UnloadGameDLL(dll) FreeLibrary(dll)
#define DLL_EXTENSION ".dll"
#else
// macOS / Linux
#include <dlfcn.h>
#define DLL_HANDLE void*
#define LoadGameDLL(path) dlopen(path, RTLD_NOW)
#define GetGameProcAddress(dll, fn) dlsym(dll, fn)
#define UnloadGameDLL(dll) dlclose(dll)
#define DLL_EXTENSION ".so"
#endif

#include <string>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "Application.hpp"
#include "InputSystem.hpp"

namespace TerracottaEngine
{
static bool LoadGameAPI(const std::string& path, DLL_HANDLE& dllHandle, GameAPI& api);
// Stuff for the game
std::string dllPath = "game" DLL_EXTENSION;
DLL_HANDLE gameDLL = nullptr;
GameAPI game;
GameState state;

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
	/*EventSystem* eventSys = m_subsystemManager->GetSubsystem<EventSystem>();
	eventSys->LinkToGLFWWindow(m_window->GetGLFWWindow());*/

	m_inputSystem = std::make_unique<InputSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_inputSystem.get());
	m_inputSystem->RegisterCallbacks(m_eventSystem.get());

	m_audioSystem = std::make_unique<AudioSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_audioSystem.get());
	m_audioSystem->PlayAudio("C:\\Windows\\Media\\Alarm05.wav");

	m_renderer = std::make_unique<Renderer>(*m_subsystemManager, *m_window);
	m_subsystemManager->RegisterSubsystem(m_renderer.get());

	// InputSystem::Init();
	/*m_eventSystem = std::make_unique<EventSystem>();
	m_eventSystem->Init();
	m_eventSystem->LinkToGLFWWindow(m_window->GetGLFWWindow());*/

	/*m_inputSystem = std::make_unique<InputSystem>();
	m_inputSystem->Init();
	m_inputSystem->RegisterCallbacks(m_eventSystem.get());*/
	
	m_layers.PushLayer(new DearImGuiLayer(m_window->GetGLFWWindow(), "Main DearImGui Layer"));

	// Check game
	if (!LoadGameAPI(dllPath, gameDLL, game)) {
		SPDLOG_ERROR("Failed to load the game's .dll/.dylib/.so file!");
		return;
	}
	game.Init(&state);

	SPDLOG_INFO("Finished creating application.");
}
Application::~Application()
{
	// InputSystem::Deinitialize();
	game.Shutdown(&state);
	UnloadGameDLL(gameDLL);
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
		game.Update(&state, deltaTime); // Update game
		update(deltaTime); // Update engine
		updateAcc -= m_targetUpdateDelay;
	}

	// Should render if enough time has elapsed (default = same as UPS)
	// USE IF STATEMENT!!!
	while (renderAcc >= m_targetFrameDelay) {
		render();
		renderAcc -= m_targetFrameDelay;
	}

	// Hot reload
	if (std::filesystem::exists("game_temp" DLL_EXTENSION)) {
		SPDLOG_WARN("Game .dll/.dylib/.so modified, hot reloading...");

		// Stop old game library
		game.Shutdown(&state);
		UnloadGameDLL(gameDLL);
#ifdef _WIN32
		// Windows: copy to a temporary file to prevent "locking"
		std::string newDLLPath = "game" DLL_EXTENSION;
		if (std::filesystem::exists(newDLLPath)) {
			std::filesystem::remove(newDLLPath);
		}
		std::filesystem::rename("game_temp" DLL_EXTENSION, newDLLPath);
		dllPath = newDLLPath;
#endif
		if (!LoadGameAPI(dllPath, gameDLL, game)) {
			SPDLOG_ERROR("Failed to reload game!");
			Stop();
		}
		game.Init(&state);
	}

	if (glfwWindowShouldClose(m_window->GetGLFWWindow())) {
		Stop();
	}
}
void Application::Stop()
{
	m_running = false;
	// Most systems are deinitialized with ~Application.
}

void Application::update(const float deltaTime)
{
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



static bool LoadGameAPI(const std::string& path, DLL_HANDLE& dllHandle, GameAPI& api)
{
	dllHandle = LoadGameDLL(path.c_str());
	if (!dllHandle) {
		SPDLOG_ERROR("Failed to load DLL: {}", path);
		return false;
	}

	api.Init = (GameInitFn)GetGameProcAddress(dllHandle, "GameInit");
	api.Update = (GameUpdateFn)GetGameProcAddress(dllHandle, "GameUpdate");
	api.Shutdown = (GameShutdownFn)GetGameProcAddress(dllHandle, "GameShutdown");

	if (!api.Init || !api.Update || !api.Shutdown) {
		SPDLOG_ERROR("Failed to get at least one function address from the game!");
		UnloadGameDLL(dllHandle);
		return false;
	}

	return true;
}
}