#include "spdlog/spdlog.h"
#include "Game.hpp"

// Game engine API
void* GameInit()
{
	TerracottaGame::Game* game = new TerracottaGame::Game();
	game->Init();
	return game;
}

void GameUpdate(void* game, float deltaTime)
{
	reinterpret_cast<TerracottaGame::Game*>(game)->Update(deltaTime);
}

void GameShutdown(void* game)
{
	reinterpret_cast<TerracottaGame::Game*>(game)->Shutdown();
}
//

namespace TerracottaGame
{
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