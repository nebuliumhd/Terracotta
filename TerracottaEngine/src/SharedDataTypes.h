#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct RenderTile
{
	float X, Y, Z;
	float ScaleX, ScaleY;
	float FrameSlotX, FrameSlotY; // Top-left UV
	float FrameSlotW, FrameSlotH; // UV width and height (ADDED)
	float TextureIndex;
} RenderTile;

typedef struct AtlasInfo
{
	uint32_t rows;
	uint32_t columns;
	float tileWidth; // UV width per tile (1.0 / columns)
	float tileHeight; // UV height per tile (1.0 / rows)
} AtlasInfo;

typedef struct UVData
{
	float MinU;
	float MinV;
	float MaxU;
	float MaxV;
} UVData;

#define CHUNK_SIZE		16
#define TILES_PER_CHUNK (CHUNK_SIZE * CHUNK_SIZE)

#ifdef __cplusplus
} // extern "C"
#endif