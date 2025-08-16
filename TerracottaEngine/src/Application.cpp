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
	m_inputSystem->RegisterCallbacks(m_eventSystem.get());

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

	SPDLOG_INFO("Finished creating application.");
}
Application::~Application()
{
	// InputSystem::Deinitialize();
}

void Application::Run()
{
	static float prevTime = (float)glfwGetTime(), updateAcc = 0.0f, renderAcc = 0.0f;

	float currTime = (float)glfwGetTime();
	const float deltaTime = currTime - prevTime;
	prevTime = currTime;
	updateAcc += deltaTime;
	renderAcc += deltaTime;

	// Should update if enough time has elapsed (default = ~0.0167ms for 60 UPS)
	// USE WHILE LOOP!!!
	while (updateAcc >= m_targetUpdateDelay) {
		processInput(); // Keep this tied to update for now
		update(deltaTime);
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
	// Most systems are deinitialized with ~Application.
}

void Application::processInput()
{
	glfwPollEvents(); // Should come first

	m_renderer->OnProcessInput();
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

	// std::this_thread::sleep_for(1000)

	glfwSwapBuffers(m_window->GetGLFWWindow());
}
}