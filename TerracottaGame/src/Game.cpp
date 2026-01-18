#include "Game.hpp"
#include "EngineConnection.hpp"
#include "spdlog/spdlog.h"

namespace TerracottaGame
{

void MainMenuState::Enter()
{
	SPDLOG_INFO("Entered Main Menu State");
}

void MainMenuState::Update(const float deltaTime)
{
	// Main menu logic here
}

void MainMenuState::Exit()
{
	SPDLOG_INFO("Exited Main Menu State");
}

Game::Game()
{}

Game::~Game()
{}

void Game::Init()
{
	SPDLOG_WARN("GAME INIT!");

	if (!Engine::IsRunning()) {
		SPDLOG_ERROR("Engine API not initialized!");
		return;
	}

	// Initialize world (10x10 chunks = 160x160 tiles)
	m_world.Init(10, 10);

	// Generate noise and populate world
	std::vector<float> randomTileData(128 * 128);
	Engine::GetNoise2D(128, 128, randomTileData.data());
	m_world.SetTileData(randomTileData);
	m_world.UpdateAllDirtyChunks();

	SPDLOG_INFO("World initialized and rendered!");
}

void Game::Update(const float deltaTime)
{
	// Check for input using the Engine helper
	if (Engine::IsKeyStartPress(81 /*GLFW_KEY_Q*/)) {
		m_data.ButtonPresses++;
		PrintData();
	}

	// Update any dirty chunks each frame
	m_world.UpdateAllDirtyChunks();

	// Update current state if we have one
	if (m_state) {
		m_state->Update(deltaTime);
	}
}

void Game::Shutdown()
{
	SPDLOG_WARN("GAME SHUTDOWN!");

	if (m_state) {
		m_state->Exit();
		delete m_state;
		m_state = nullptr;
	}
}

void Game::PrintData()
{
	SPDLOG_INFO("We have {} button presses.", m_data.ButtonPresses);
}

} // namespace TerracottaGame