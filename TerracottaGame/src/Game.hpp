#pragma once
#include "World.hpp"

namespace TerracottaGame
{
class GameState
{
public:
	virtual ~GameState() {}
	virtual void Enter() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Exit() = 0;
};

class MainMenuState : public GameState
{
public:
	~MainMenuState() override {}
	void Enter() override;
	void Update(const float deltaTime) override;
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
	void Update(const float deltaTime);
	void Shutdown();

	void PrintData();

	GameData& GetGameData() { return m_data; }
	void SetGameData(GameData data) { m_data = data; }
private:
	World m_world;
	GameState* m_state = nullptr;
	GameData m_data;
};
} // namespace TerracottaGame