#include "World.hpp"
#include "EngineConnection.hpp" // For Engine:: and g_engineAPI
#include "spdlog/spdlog.h"

namespace TerracottaGame
{

World::World()
{}

World::~World()
{}

void World::Init(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks)
{
	m_worldWidthInChunks = worldWidthInChunks;
	m_worldHeightInChunks = worldHeightInChunks;

	// Initialize renderer
	Engine::InitWorldRendering(worldWidthInChunks, worldHeightInChunks);

	// Load terrain atlas (6x9 grid)
	m_terrainAtlasId = Engine::LoadTextureAtlas("../../../../../TerracottaGame/res/tileset/tiles01.png");

	// Get atlas info for UV calculation
	if (!Engine::GetAtlasInfo(m_terrainAtlasId, &m_terrainAtlasInfo)) {
		SPDLOG_ERROR("Failed to get atlas info!");
		return;
	}

	SPDLOG_WARN("Terrain atlas loaded: {}x{} tiles, UV size: {}x{}", m_terrainAtlasInfo.rows, m_terrainAtlasInfo.columns, m_terrainAtlasInfo.tileWidth, m_terrainAtlasInfo.tileHeight);

	// Create chunks
	uint32_t totalChunks = worldWidthInChunks * worldHeightInChunks;
	m_chunks.reserve(totalChunks);

	for (uint32_t y = 0; y < worldHeightInChunks; ++y) {
		for (uint32_t x = 0; x < worldWidthInChunks; ++x) {
			m_chunks.emplace_back(x, y);
		}
	}

	SPDLOG_INFO("World initialized: {}x{} chunks", worldWidthInChunks, worldHeightInChunks);
}

void World::UpdateChunkRendering(uint32_t chunkX, uint32_t chunkY)
{
	if (!Engine::IsRunning())
		return;

	Chunk* chunk = GetChunk(chunkX, chunkY);
	if (!chunk)
		return;

	const GameTile* gameTiles = chunk->GetTiles();
	constexpr uint32_t tilesPerChunk = TILES_PER_CHUNK;
	RenderTile renderTiles[tilesPerChunk];

	for (uint32_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y) {
		for (uint32_t x = 0; x < Chunk::CHUNK_WIDTH; ++x) {
			uint32_t idx = y * Chunk::CHUNK_WIDTH + x;
			const GameTile& gameTile = gameTiles[idx];

			// World position
			float worldX = static_cast<float>(chunkX * Chunk::CHUNK_WIDTH + x);
			float worldY = static_cast<float>(chunkY * Chunk::CHUNK_HEIGHT + y);

			renderTiles[idx].X = worldX;
			renderTiles[idx].Y = worldY;
			renderTiles[idx].Z = 0.0f;
			renderTiles[idx].ScaleX = 1.0f;
			renderTiles[idx].ScaleY = 1.0f;

			// Get UV coordinates from atlas (with insets built-in!)
			uint32_t tileId = static_cast<uint32_t>(gameTile.Type);
			UVData uvs;
			Engine::GetTileUVs(m_terrainAtlasId, tileId, &uvs);

			renderTiles[idx].FrameSlotX = uvs.MinU;
			renderTiles[idx].FrameSlotY = uvs.MinV;
			renderTiles[idx].FrameSlotW = uvs.MaxU - uvs.MinU;
			renderTiles[idx].FrameSlotH = uvs.MaxV - uvs.MinV;
			renderTiles[idx].TextureIndex = static_cast<float>(m_terrainAtlasId);
		}
	}

	// Debug first tile
	if (chunkX == 0 && chunkY == 0) {
		SPDLOG_INFO("Tile UV WITH INSETS: ({}, {}) to ({}, {})", renderTiles[0].FrameSlotX, renderTiles[0].FrameSlotY, renderTiles[0].FrameSlotX + renderTiles[0].FrameSlotW,
			renderTiles[0].FrameSlotY + renderTiles[0].FrameSlotH);
	}

	// Send to engine
	Engine::UpdateChunkTiles(chunkX, chunkY, renderTiles, tilesPerChunk);
	chunk->ClearDirty();
}

void World::UpdateAllDirtyChunks()
{
	// Iterate through all chunks and update dirty ones
	for (uint32_t y = 0; y < m_worldHeightInChunks; ++y) {
		for (uint32_t x = 0; x < m_worldWidthInChunks; ++x) {
			Chunk* chunk = GetChunk(x, y);
			if (chunk && chunk->IsDirty()) {
				UpdateChunkRendering(x, y);
			}
		}
	}
}

void World::SetTileData(const std::vector<float>& data)
{
	// Use noise data to set tile types
	for (size_t i = 0; i < data.size() && i < m_chunks.size() * Chunk::CHUNK_TILE_COUNT; ++i) {
		uint32_t chunkIndex = static_cast<uint32_t>(i) / Chunk::CHUNK_TILE_COUNT;
		uint32_t tileIndex = static_cast<uint32_t>(i) % Chunk::CHUNK_TILE_COUNT;

		if (chunkIndex < m_chunks.size()) {
			GameTile* tiles = const_cast<GameTile*>(m_chunks[chunkIndex].GetTiles());

			// Convert noise value to tile type
			// TODO: Move this to a separate file???
			float noiseValue = data[i];
			if (noiseValue > 0.5f) {
				tiles[tileIndex].Type = TileType::ROCK;
			} else {
				tiles[tileIndex].Type = TileType::GRASS;
			}
			tiles[tileIndex].Variant = 0;
		}
	}

	// Mark all chunks dirty so they get re-rendered
	for (auto& chunk : m_chunks) {
		const_cast<Chunk&>(chunk).MarkDirty();
	}
}

GameTile* World::GetTile(uint32_t worldX, uint32_t worldY)
{
	uint32_t chunkX = worldX / Chunk::CHUNK_WIDTH;
	uint32_t chunkY = worldY / Chunk::CHUNK_HEIGHT;
	uint32_t localX = worldX % Chunk::CHUNK_WIDTH;
	uint32_t localY = worldY % Chunk::CHUNK_HEIGHT;

	Chunk* chunk = GetChunk(chunkX, chunkY);
	if (!chunk)
		return nullptr;

	uint32_t tileIndex = localY * Chunk::CHUNK_WIDTH + localX;
	return const_cast<GameTile*>(&chunk->GetTiles()[tileIndex]);
}

Chunk* World::GetChunk(uint32_t chunkX, uint32_t chunkY)
{
	uint32_t index = getChunkIndex(chunkX, chunkY);
	if (index >= m_chunks.size())
		return nullptr;
	return &m_chunks[index];
}

uint32_t World::getChunkIndex(uint32_t chunkX, uint32_t chunkY) const
{
	return chunkY * m_worldWidthInChunks + chunkX;
}

} // namespace TerracottaGame