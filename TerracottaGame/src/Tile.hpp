#pragma once
#include <cstdint>
#include "glm/glm.hpp"

namespace TerracottaGame
{
enum class TileType : uint8_t
{
	GRASS = 0,
	ROCK = 1
};

struct GameTile
{
	TileType Type;
	uint8_t Variant;
};
} // namespace TerracottaGame