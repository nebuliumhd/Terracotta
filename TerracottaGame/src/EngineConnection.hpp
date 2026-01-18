#pragma once
#include "EngineAPI.h"

// Global API pointer defined in GameAPI.cpp
extern const EngineAPI* g_engineAPI;

namespace TerracottaGame
{
class Engine
{
public:
	static bool IsRunning() { return g_engineAPI != nullptr; }

	// World/Rendering
	static void InitWorldRendering(uint32_t w, uint32_t h)
	{
		if (g_engineAPI)
			g_engineAPI->InitWorldRendering(w, h);
	}

	static void UpdateChunkTiles(uint32_t x, uint32_t y, const RenderTile* tiles, uint32_t count)
	{
		if (g_engineAPI)
			g_engineAPI->UpdateChunkTiles(x, y, tiles, count);
	}

	static uint32_t LoadTextureAtlas(const char* path) { return g_engineAPI ? g_engineAPI->LoadTextureAtlas(path) : 0; }

	static bool GetAtlasInfo(uint32_t atlasId, AtlasInfo* outInfo) { return g_engineAPI ? g_engineAPI->GetAtlasInfo(atlasId, outInfo) != 0 : false; }

	static void GetTileUVs(uint32_t atlasId, uint32_t tileId, UVData* uvs)
	{
		if (g_engineAPI)
			g_engineAPI->GetTileUVs(atlasId, tileId, uvs);
	}

	// Noise
	static void GetNoise2D(uint32_t w, uint32_t h, float* data)
	{
		if (g_engineAPI)
			g_engineAPI->GetNoise2D(w, h, data);
	}

	// Input
	static bool IsKeyDown(int keyCode) { return g_engineAPI ? g_engineAPI->IsKeyDown(keyCode) != 0 : false; }

	static bool IsKeyStartPress(int keyCode) { return g_engineAPI ? g_engineAPI->IsKeyStartPress(keyCode) != 0 : false; }

	static bool IsKeyEndPress(int keyCode) { return g_engineAPI ? g_engineAPI->IsKeyEndPress(keyCode) != 0 : false; }

	// Mouse
	static void GetMousePosition(float* outX, float* outY)
	{
		if (g_engineAPI)
			g_engineAPI->GetMousePosition(outX, outY);
	}

	static bool IsMouseButtonDown(int button) { return g_engineAPI ? g_engineAPI->IsMouseButtonDown(button) != 0 : false; }

	// Other
	static float GetTotalTime() { return g_engineAPI ? g_engineAPI->GetTotalTime() : 0.0f; }
};
} // namespace TerracottaGame