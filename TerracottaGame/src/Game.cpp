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
	// s_engineAPI->Log(s_engineAPI->Instance, "HELLO FROM GAME WOW!!!");
}
void GameShutdown(GameHandle game)
{
	reinterpret_cast<Game*>(game)->Shutdown();
	s_engineAPI = nullptr;
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
	
}
void Game::Shutdown()
{
	SPDLOG_WARN("GAME SHUTDOWN!");
}
}