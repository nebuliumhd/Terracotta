#include "spdlog/spdlog.h"
#include "GameAPI.hpp"

namespace TerracottaGame
{
namespace TE = TerracottaEngine;
// Game currently keeps track of one engine instance
static TE::EngineAPI* s_engineAPI;

// Game engine API
GameHandle GameInit(TE::EngineAPI* engine)
{
	Game* game = new Game();
	s_engineAPI = engine;
	game->Init();
	return game;
}
void GameUpdate(GameHandle game, float deltaTime)
{
	reinterpret_cast<Game*>(game)->Update(deltaTime);
}
void GameShutdown(GameHandle game)
{
	reinterpret_cast<Game*>(game)->Shutdown();
	s_engineAPI = nullptr;
}
void* GameSerializeState(GameHandle game, size_t* outSize)
{
	Game* g = reinterpret_cast<Game*>(game);

	const GameData& data = g->GetGameData();
	size_t dataSize = sizeof(GameData);
	void* dataBuffer = malloc(dataSize);
	if (!dataBuffer) {
		SPDLOG_ERROR("Failed to allocate memory to serialize the GameData!");
		*outSize = 0;
		return nullptr;
	}

	memcpy(dataBuffer, &data, dataSize);
	*outSize = dataSize;

	SPDLOG_INFO("Serialized game state: {} bytes.", dataSize);
	return dataBuffer;
}
GameHandle GameDeserializeState(TE::EngineAPI* engine, void* data, size_t size)
{
	Game* game = new Game();
	s_engineAPI = engine;

	// Deserialize state from buffer into the game object
	GameData* savedData = reinterpret_cast<GameData*>(data);
	if (savedData) {
		game->SetGameData(*savedData);
		game->PrintData();
	} else {
		SPDLOG_ERROR("Failed to get the previously saved data!");
	}

	return game;
}

void MainMenuState::Enter()
{

}
void MainMenuState::Update(float deltaTime)
{

}
void MainMenuState::Exit()
{

}

Game::Game()
{
	SPDLOG_INFO("GAME HAS BEEN CREATED!");
}
Game::~Game()
{
	SPDLOG_INFO("GAME OVER!");
}

void Game::Init()
{
	SPDLOG_WARN("GAME INIT!");
}
void Game::Update(float deltaTime)
{
	if (s_engineAPI->IsKeyStartPress(s_engineAPI->Instance, 81 /*GLFW_KEY_Q*/)) {
		m_data.ButtonPresses++;
		PrintData();
	}
}
void Game::Shutdown()
{
	SPDLOG_WARN("GAME SHUTDOWN!");
}

void Game::PrintData()
{
	SPDLOG_INFO("We have {} button presses. Cool.", m_data.ButtonPresses);
}
}