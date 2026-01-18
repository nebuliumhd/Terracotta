#pragma once
#include <cstdint>
#include "glm/glm.hpp"
#include "Tile.hpp"

namespace TerracottaGame
{

class Chunk
{
public:
	static constexpr uint32_t CHUNK_WIDTH = 16;
	static constexpr uint32_t CHUNK_HEIGHT = 16;
	static constexpr uint32_t CHUNK_TILE_COUNT = CHUNK_WIDTH * CHUNK_HEIGHT;

	Chunk(uint32_t x, uint32_t y);
	~Chunk();

	const GameTile* GetTiles() const;
	glm::ivec2 GetPosition() const;
	bool IsDirty() const;
	void ClearDirty();
	void MarkDirty();
private:
	GameTile m_tiles[CHUNK_TILE_COUNT];
	float m_x, m_y;
	bool m_dirty = true;
};

} // namespace TerracottaGame