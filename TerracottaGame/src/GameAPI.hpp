#pragma once

#include "EngineAPI.hpp"

#ifdef _MSC_VER
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __attribute__((visibility("default")))
#endif

using GameHandle = void*;

namespace TerracottaGame
{
// Stable ABI for hot reloading
extern "C" {
	GAME_API GameHandle GameInit(TerracottaEngine::EngineAPI* engine);
	GAME_API void GameUpdate(GameHandle game, float deltaTime);
	GAME_API void GameShutdown(GameHandle game);
}

class GameState
{
public:
	virtual ~GameState() {}
	virtual void Enter() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Exit() = 0;
};

class MainMenuState : public GameState
{
public:
	~MainMenuState() override {}
	void Enter() override;
	void Update(float deltaTime) override;
	void Exit() override;
};

class Game
{
public:
	Game();
	~Game();

	void Init();
	void Update(float deltaTime);
	void Shutdown();
private:
	GameState* m_state = nullptr;
};
}