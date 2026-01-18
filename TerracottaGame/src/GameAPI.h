#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "EngineAPI.h"

// Opaque game instance handle
typedef void* GameInstance;

// Game DLL exports - the ENGINE calls these
#ifdef _WIN32
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __attribute__((visibility("default")))
#endif

// Required exports
GAME_API GameInstance GameInit(const EngineAPI* engineAPI);
GAME_API void GameUpdate(GameInstance instance, float deltaTime);
GAME_API void GameShutdown(GameInstance instance);

// Optional hot-reload exports
GAME_API void* GameSerializeState(GameInstance instance, size_t* outSize);
GAME_API GameInstance GameDeserializeState(const EngineAPI* engineAPI, const void* data, size_t size);

#ifdef __cplusplus
} // extern "C"
#endif