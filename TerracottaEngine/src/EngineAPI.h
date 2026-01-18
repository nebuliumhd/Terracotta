#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "SharedDataTypes.h"

typedef struct EngineAPI
{
	void (*InitWorldRendering)(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks);
	void (*UpdateChunkTiles)(uint32_t chunkX, uint32_t chunkY, const RenderTile* tiles, uint32_t tileCount);
	uint32_t (*LoadTextureAtlas)(const char* path);
	int (*GetAtlasInfo)(uint32_t atlasId, AtlasInfo* outInfo);
	void (*GetTileUVs)(uint32_t atlasId, uint32_t tileId, UVData* outData);
	void (*GetNoise2D)(uint32_t width, uint32_t height, float* outData);
	int (*IsKeyDown)(int keyCode);
	int (*IsKeyStartPress)(int keyCode);
	int (*IsKeyEndPress)(int keyCode);
	void (*GetMousePosition)(float* outX, float* outY);
	int (*IsMouseButtonDown)(int button);
	float (*GetTotalTime)(void);
} EngineAPI;

EngineAPI EngineAPI_Create(void* appPtr);

#ifdef __cplusplus
} // extern "C"
#endif