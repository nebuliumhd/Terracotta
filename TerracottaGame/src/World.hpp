#pragma once
#include <filesystem>
#include <cstdint>
#include <vector>
#include "Chunk.hpp"
#include "SharedDataTypes.h"

namespace TerracottaGame
{

using Filepath = std::filesystem::path;

class World
{
public:
	World();
	~World();

	void Init(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks);
	void UpdateChunkRendering(uint32_t chunkX, uint32_t chunkY);
	void UpdateAllDirtyChunks();

	void SetTileData(const std::vector<float>& data);
	GameTile* GetTile(uint32_t worldX, uint32_t worldY);
	Chunk* GetChunk(uint32_t chunkX, uint32_t chunkY);
private:
	std::vector<Chunk> m_chunks;
	// Atlas tracking
	AtlasInfo m_terrainAtlasInfo = {};
	uint32_t m_terrainAtlasId = 0;
	uint32_t m_worldWidthInChunks = 0;
	uint32_t m_worldHeightInChunks = 0;

	uint32_t getChunkIndex(uint32_t chunkX, uint32_t chunkY) const;
};
} // namespace TerracottaGame