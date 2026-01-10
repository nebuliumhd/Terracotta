#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "glm/glm.hpp"

namespace TerracottaEngine
{
using json = nlohmann::json;
using Filepath = std::filesystem::path;

struct TilemapData
{
	std::string Name;
	int Width, Height;
	Filepath AtlasPath;
	int AtlasRows, AtlasColumns;
	std::vector<uint32_t> Tiles;
};

class JSONParser
{
public:
	JSONParser();
	~JSONParser();

	TilemapData LoadTilemapFromFile(const Filepath& path);
	void SaveTilemapToFile(const TilemapData& tilepath, const Filepath& path);
private:
	json m_data;
};
}