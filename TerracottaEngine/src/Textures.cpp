#include "spdlog/spdlog.h"
#include "stb/stb_image.h"
#include "JSONParser.hpp"
#include "Textures.hpp"

namespace TerracottaEngine
{
Texture::Texture(const Filepath& texturePath) :
	m_dimensions(glm::vec2(0.0f))
{
	std::string textureStr = texturePath.string();
	if (!std::filesystem::exists(texturePath)) {
		SPDLOG_INFO("The texture file {} does not exist.", textureStr);
		return;
	}

	int imgWidth, imgHeight, numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imgData = stbi_load(textureStr.c_str(), &imgWidth, &imgHeight, &numChannels, 0);

	if (!imgData) {
		SPDLOG_ERROR("Failed to load texture file at \"{}\".", textureStr);
		return;
	}

	m_dimensions = glm::vec2((float)imgWidth, (float)imgHeight);
	SPDLOG_INFO("Loaded texture file at {} of {{{} x {}}} with {} color channels.", textureStr, m_dimensions.x, m_dimensions.y, numChannels);

	// Upload image to OpenGL and free resources
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLenum format = (numChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imgData);
}
Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

TextureAtlas::TextureAtlas(const Filepath& atlasPath) :
	m_atlas(atlasPath)
{
	// We know where all of the metadata for tilesets are stored. Use the filename of the atlas to view the JSON
	AtlasInfo info = JSONParser::LoadAtlasInfo(atlasPath);
	m_rows = info.rows;
	m_columns = info.columns;
	m_tileWidth = 1.0f / m_columns;
	m_tileHeight = 1.0f / m_rows;
}
TextureAtlas::~TextureAtlas()
{}

// Textures.cpp
glm::vec4 TextureAtlas::GetTileUVs(int tileId) const
{
	if (tileId < 0 || tileId >= GetTileCount()) {
		SPDLOG_WARN("Invalid tile ID: {}", tileId);
		return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	// Calculate column (left to right)
	int column = tileId % m_columns;
	// We have to flip the rows to calculate normally
	int rowFromBottom = tileId / m_columns;
	int row = (m_rows - 1) - rowFromBottom; // Flip the row

	// Calculate pixel size in UV space
	float atlasWidth = static_cast<float>(m_atlas.GetWidth());
	float atlasHeight = static_cast<float>(m_atlas.GetHeight());
	float pixelU = 1.0f / atlasWidth;
	float pixelV = 1.0f / atlasHeight;

	// Inset by 0.5 pixels on each side to avoid bleeding
	float inset = 0.5f;
	float minU = column * m_tileWidth + (pixelU * inset);
	float minV = row * m_tileHeight + (pixelV * inset);
	float maxU = (column + 1) * m_tileWidth - (pixelU * inset);
	float maxV = (row + 1) * m_tileHeight - (pixelV * inset);

	return glm::vec4(minU, minV, maxU, maxV);
}
} // namespace TerracottaEngine