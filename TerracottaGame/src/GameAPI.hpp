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

	// Save game data
	GAME_API void* GameSerializeState(GameHandle game, size_t* outSize);
	GAME_API GameHandle GameDeserializeState(TerracottaEngine::EngineAPI* engine, void* data, size_t size);
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

struct GameData
{
	int ButtonPresses = 0;
};

class Game
{
public:
	Game();
	~Game();

	void Init();
	void Update(float deltaTime);
	void Shutdown();

	void PrintData();

	GameData& GetGameData() { return m_data; }
	void SetGameData(GameData data) { m_data = data; }
private:
	GameState* m_state = nullptr;
	GameData m_data;
};
}