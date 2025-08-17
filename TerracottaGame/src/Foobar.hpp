#pragma once

#if defined(_MSC_VER)
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __attribute__((visibility("default")))
#endif

struct GameState
{
	int score;
	float X, Y;
};

extern "C" {
	GAME_API void GameInit(GameState* state);
	GAME_API void GameUpdate(GameState* state);
	GAME_API void GameShutdown(GameState* state);
}