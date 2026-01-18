#include "GameAPI.h"
#include "Game.hpp"
#include "spdlog/spdlog.h"
#include <cstring>

namespace TG = TerracottaGame;

// Definition pointer
const EngineAPI* g_engineAPI = nullptr;

extern "C" {
GAME_API GameInstance GameInit(const EngineAPI* engineAPI)
{
	SPDLOG_INFO("GameInit called");

	// Store the engine API globally
	g_engineAPI = engineAPI;

	// Create and initialize the game
	TG::Game* game = new TG::Game();
	game->Init();

	return game;
}

GAME_API void GameUpdate(GameInstance instance, float deltaTime)
{
	if (!instance) {
		SPDLOG_ERROR("GameUpdate called with null instance!");
		return;
	}

	TG::Game* game = static_cast<TG::Game*>(instance);
	game->Update(deltaTime);
}

GAME_API void GameShutdown(GameInstance instance)
{
	if (!instance) {
		SPDLOG_ERROR("GameShutdown called with null instance!");
		return;
	}

	SPDLOG_INFO("GameShutdown called");

	TG::Game* game = static_cast<TG::Game*>(instance);
	game->Shutdown();
	delete game;

	// Clear the global API pointer
	g_engineAPI = nullptr;
}

GAME_API void* GameSerializeState(GameInstance instance, size_t* outSize)
{
	if (!instance || !outSize) {
		SPDLOG_ERROR("GameSerializeState called with invalid parameters!");
		return nullptr;
	}

	TG::Game* game = static_cast<TG::Game*>(instance);
	const TG::GameData& data = game->GetGameData();

	// Allocate buffer and copy data
	size_t dataSize = sizeof(TG::GameData);
	void* buffer = malloc(dataSize);
	if (!buffer) {
		SPDLOG_ERROR("Failed to allocate serialization buffer");
		*outSize = 0;
		return nullptr;
	}

	memcpy(buffer, &data, dataSize);
	*outSize = dataSize;

	SPDLOG_INFO("Serialized game state: {} bytes", *outSize);
	return buffer;
}

GAME_API GameInstance GameDeserializeState(const EngineAPI* engineAPI, const void* data, size_t size)
{
	SPDLOG_INFO("GameDeserializeState called with {} bytes", size);

	// Store the engine API globally
	g_engineAPI = engineAPI;

	// Create new game instance
	TG::Game* game = new TG::Game();
	game->Init();

	// Restore state if valid
	if (data && size == sizeof(TG::GameData)) {
		const TG::GameData* savedData = static_cast<const TG::GameData*>(data);
		game->SetGameData(*savedData);
		game->PrintData();
		SPDLOG_INFO("Restored game state from serialized data");
	} else {
		SPDLOG_WARN("Invalid serialized data (expected {} bytes, got {}), starting with fresh state", sizeof(TG::GameData), size);
	}

	return game;
}

} // extern "C"