#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "glm/glm.hpp"
#include "SharedDataTypes.h"

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
	std::vector<uint32_t> Tiles = {0};
};

// Can either be created with a json state or used as a temp object
class JSONParser
{
public:
	static AtlasInfo LoadAtlasInfo(const Filepath& path);
	static TilemapData LoadTilemapFromFile(const Filepath& path);
	static void SaveTilemapToFile(const TilemapData& tilepath, const Filepath& path);
private:
	JSONParser() = delete;
	~JSONParser() = delete;
};
} // namespace TerracottaEngine