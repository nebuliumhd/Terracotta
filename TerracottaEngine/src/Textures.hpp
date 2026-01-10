#pragma once

#include <filesystem>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace TerracottaEngine
{
using Filepath = std::filesystem::path;

class Texture
{
public:
	Texture(const Filepath& texturePath);
	~Texture();

	GLuint GetID() const { return m_id; }
	void Bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
	glm::vec2 GetDimensions() const { return m_dimensions; }
private:
	GLuint m_id = 0;
	glm::vec2 m_dimensions; // WxH of texture atlas
};

class TextureAtlas
{
public:
	TextureAtlas(const Filepath& atlasPath, int rows, int columns);
	~TextureAtlas();

	glm::vec2 GetAtlasDimensions() const { return m_atlas.GetDimensions(); }

	// Gets the UV coordinates for a specific tile from the atlas
	glm::vec4 GetTileUVs(int tildId) const;
	
	const Texture& GetTexture() const { return m_atlas; }
	Texture& GetTexture() { return m_atlas; }
	int GetRows() const { return m_rows; }
	int GetColumns() const { return m_columns; }
	int GetTileCount() const { return m_rows * m_columns; }
private:
	Texture m_atlas; // Actual OpenGL texture
	int m_rows, m_columns;
	float m_tileWidth, m_tileHeight; // UV width (1.0 / columns), (1.0 / rows)
};
}