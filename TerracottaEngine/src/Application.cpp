// Hot reload for Unix operating systems
#include <string>
#include "spdlog/spdlog.h"
#include "Application.hpp"
#include "InputSystem.hpp"

namespace TerracottaEngine
{
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
	m_inputSystem->Init();

	m_audioSystem = std::make_unique<AudioSystem>(*m_subsystemManager);
	m_subsystemManager->RegisterSubsystem(m_audioSystem.get());
	// Load the audio system in the future
	UUIDv4::UUID songID = m_audioSystem->LoadAudio("../../../../../TerracottaEngine/res/audio.ogg");
	m_audioSystem->PlayAudio(songID, ChannelGroupID::Master);

	m_renderer = std::make_unique<Renderer>(*m_subsystemManager, *m_window);
	m_subsystemManager->RegisterSubsystem(m_renderer.get());
	
	m_layers.PushLayer(new DearImGuiLayer(m_window->GetGLFWWindow(), "Main DearImGui Layer"));

	// Game initialization
	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to load the game's shared library!");
		assert(0);
	}
	m_gameAPI.Init();
	SPDLOG_INFO("Loaded the game's shared library.");

	SPDLOG_INFO("Finished creating application.");
}
Application::~Application()
{

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
		m_gameAPI.Update(m_gameInstance, deltaTime);
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

	m_dllHandle = LOAD_DLL(dllName);
	if (!m_dllHandle) {
		SPDLOG_ERROR("Failed to get the handle for the game DLL! Make sure it's called game.dll/libgame.dylib/libgame.so");
		return false;
	}

	m_gameAPI.Init = (GameInitFn)GET_DLL_SYMBOL(m_dllHandle, "GameInit");
	m_gameAPI.Update = (GameUpdateFn)GET_DLL_SYMBOL(m_dllHandle, "GameUpdate");
	m_gameAPI.Shutdown = (GameShutdownFn)GET_DLL_SYMBOL(m_dllHandle, "GameShutdown");

	if (!m_gameAPI.Init || !m_gameAPI.Update || !m_gameAPI.Shutdown) {
		SPDLOG_ERROR("Failed to load one or more functions from the game DLL!");
		UNLOAD_DLL(m_dllHandle);
		m_dllHandle = nullptr;
		return false;
	}
	
	return true;
}
void Application::unloadGameDLL()
{
	if (!m_dllHandle)
		return;

	UNLOAD_DLL(m_dllHandle);
	m_dllHandle = nullptr;

	m_gameAPI.Init = nullptr;
	m_gameAPI.Update = nullptr;
	m_gameAPI.Shutdown = nullptr;

	SPDLOG_INFO("Shut down the game!");
}
void Application::reloadGameDLL()
	{
	m_gameAPI.Shutdown(m_gameInstance);

	UNLOAD_DLL(m_dllHandle);

	// Wait just so the OS can catch up with writing
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	if (!loadGameDLL()) {
		SPDLOG_ERROR("Failed to reload the DLL!");
		return;
	}

	m_gameInstance = m_gameAPI.Init();
}
}