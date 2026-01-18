#include "EngineAPI.h"
#include "Application.hpp"
#include "spdlog/spdlog.h"

namespace TerracottaEngine
{

// Thread-local context that stores current Application pointer and deltaTime
struct EngineContext
{
	Application* app = nullptr;
};

static EngineContext g_context;

// Internal helpers to get current application
static Application* GetApp()
{
	return g_context.app;
}

// EngineAPI struct functions

static void Impl_InitWorldRendering(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks)
{
	if (Application* app = GetApp()) {
		app->GetRenderer()->InitChunkProxies(worldWidthInChunks, worldHeightInChunks);
	}
}

static void Impl_UpdateChunkTiles(uint32_t chunkX, uint32_t chunkY, const RenderTile* tiles, uint32_t tileCount)
{
	if (Application* app = GetApp()) {
		app->GetRenderer()->UpdateChunkTiles(chunkX, chunkY, reinterpret_cast<const RenderTile*>(tiles), tileCount);
	}
}

static uint32_t Impl_LoadTextureAtlas(const char* path)
{
	if (Application* app = GetApp()) {
		return app->GetRenderer()->LoadAndAddTextureAtlas(path);
	}
	return 0;
}

static void Impl_GetNoise2D(uint32_t width, uint32_t height, float* outData)
{
	if (Application* app = GetApp()) {
		app->GetRandomGenerator()->GetNoise2D(width, height, outData);
	}
}

static int Impl_IsKeyDown(int keyCode)
{
	if (Application* app = GetApp()) {
		return app->GetInputSystem()->IsKeyDown(keyCode) ? 1 : 0;
	}
	return 0;
}

static int Impl_IsKeyStartPress(int keyCode)
{
	if (Application* app = GetApp()) {
		return app->GetInputSystem()->IsKeyStartPress(keyCode) ? 1 : 0;
	}
	return 0;
}

static int Impl_IsKeyEndPress(int keyCode)
{
	if (Application* app = GetApp()) {
		return app->GetInputSystem()->IsKeyEndPress(keyCode) ? 1 : 0;
	}
	return 0;
}

static void Impl_GetMousePosition(float* outX, float* outY)
{
	if (Application* app = GetApp()) {
		// auto pos = app->GetInputSystem()->GetMousePosition();
		glm::vec2 pos = {0.0f, 0.0f};
		if (outX)
			*outX = pos.x;
		if (outY)
			*outY = pos.y;
	}
}

static int Impl_IsMouseButtonDown(int button)
{
	if (Application* app = GetApp()) {
		return app->GetInputSystem()->IsMouseButtonDown(button) ? 1 : 0;
	}
	return 0;
}

static float Impl_GetTotalTime(void)
{
	return static_cast<float>(glfwGetTime());
}

static int Impl_GetAtlasInfo(uint32_t atlasId, AtlasInfo* outInfo)
{
	if (!outInfo)
		return 0;

	if (Application* app = GetApp()) {
		return app->GetRenderer()->GetAtlasInfo(atlasId, outInfo);
	}
	return 0;
}

static void Impl_GetTileUVs(uint32_t atlasId, uint32_t tileId, UVData* outData)
{
	if (!outData)
		return;

	if (Application* app = GetApp()) {
		app->GetRenderer()->GetTileUVs(atlasId, tileId, outData);
	}
}

} // namespace TerracottaEngine

extern "C" {

EngineAPI EngineAPI_Create(void* appPtr)
{
	TerracottaEngine::g_context.app = static_cast<TerracottaEngine::Application*>(appPtr);

	EngineAPI api;
	api.InitWorldRendering = TerracottaEngine::Impl_InitWorldRendering;
	api.UpdateChunkTiles = TerracottaEngine::Impl_UpdateChunkTiles;
	api.LoadTextureAtlas = TerracottaEngine::Impl_LoadTextureAtlas;
	api.GetAtlasInfo = TerracottaEngine::Impl_GetAtlasInfo;
	api.GetTileUVs = TerracottaEngine::Impl_GetTileUVs;
	api.GetNoise2D = TerracottaEngine::Impl_GetNoise2D;
	api.IsKeyDown = TerracottaEngine::Impl_IsKeyDown;
	api.IsKeyStartPress = TerracottaEngine::Impl_IsKeyStartPress;
	api.IsKeyEndPress = TerracottaEngine::Impl_IsKeyEndPress;
	api.GetMousePosition = TerracottaEngine::Impl_GetMousePosition;
	api.IsMouseButtonDown = TerracottaEngine::Impl_IsMouseButtonDown;
	api.GetTotalTime = TerracottaEngine::Impl_GetTotalTime;

	return api;
}

} // extern "C"