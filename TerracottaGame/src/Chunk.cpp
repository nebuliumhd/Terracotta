#include "Chunk.hpp"

namespace TerracottaGame
{

Chunk::Chunk(uint32_t x, uint32_t y) :
	m_x(static_cast<float>(x)), m_y(static_cast<float>(y)), m_dirty(true)
{
	// Initialize all tiles to grass by default
	for (uint32_t i = 0; i < CHUNK_TILE_COUNT; ++i) {
		m_tiles[i].Type = TileType::GRASS;
		m_tiles[i].Variant = 0;
	}
}

Chunk::~Chunk()
{}

const GameTile* Chunk::GetTiles() const
{
	return m_tiles;
}

glm::ivec2 Chunk::GetPosition() const
{
	return glm::ivec2(m_x, m_y);
}

bool Chunk::IsDirty() const
{
	return m_dirty;
}

void Chunk::ClearDirty()
{
	m_dirty = false;
}

void Chunk::MarkDirty()
{
	m_dirty = true;
}

} // namespace TerracottaGame