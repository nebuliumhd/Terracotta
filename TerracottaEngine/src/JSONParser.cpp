#include <fstream>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"
#include "VertexInput.hpp"
#include "Renderer.hpp"
#include "JSONParser.hpp"

namespace TerracottaEngine
{
JSONParser::JSONParser()
{}
JSONParser::~JSONParser()
{}

TilemapData JSONParser::LoadTilemapFromFile(const Filepath& path)
{
	TilemapData result;

	std::ifstream jsonFile(path);
	if (!jsonFile) {
		SPDLOG_ERROR("Failed to open JSON file from \"{}\"", path.string());
		return result;
	}

	json data = json::parse(jsonFile);

	// Map data
	result.Name = data["map"]["name"];
	result.Width = data["map"]["width"];
	result.Height = data["map"]["height"];

	// Texture atlas data
	result.AtlasPath = std::string(data["tileset"]["atlas"]);
	result.AtlasRows = data["tileset"]["rows"];
	result.AtlasColumns = data["tileset"]["columns"];

	// Actual tile data
	result.Tiles = data["layers"][0]["data"].get<std::vector<uint32_t>>();

	SPDLOG_INFO("Parsed tilemap JSON \"{}\": {}x{} tiles", result.Name, result.Width, result.Height);
	
	return result;
}

void JSONParser::SaveTilemapToFile(const TilemapData& tilemap, const Filepath& path)
{
	if (std::filesystem::exists(path)) {
		SPDLOG_WARN("JSON file {} already exists... Overwriting existing data.");
	}

	std::ofstream jsonFile(path);
	if (!jsonFile) {
		SPDLOG_ERROR("Failed to create JSON file \"{}\"", path.string());
		return;
	}

	json data;

	data["map"]["name"] = tilemap.Name;
	data["map"]["width"] = tilemap.Width;
	data["map"]["height"] = tilemap.Height;
	
	data["tileset"]["atlas"] = tilemap.AtlasPath;
	data["tileset"]["rows"] = tilemap.AtlasRows;
	data["tileset"]["columns"] = tilemap.AtlasColumns;
	
	data["layers"][0]["name"] = "h";
	data["layers"][0]["data"] = tilemap.Tiles;

	jsonFile << data.dump(4);
}
}