#pragma once

#include <filesystem>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace TerracottaEngine
{
class Texture
{
public:
	Texture(const std::string& texturePath);
	~Texture();

	void SetSlot(GLenum slot) { m_slot = slot; }
	GLenum GetSlot() const { return m_slot; }

	void Activate() const { glActiveTexture(m_slot); }
	void Bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
	glm::vec2 GetDimensions() const { return m_dimensions; }
private:
	GLuint m_id = 0;
	GLenum m_slot = GL_TEXTURE0;
	glm::vec2 m_dimensions; // WxH of texture atlas
};

class TextureAtlas
{
public:
	TextureAtlas(const std::string& atlasPath);
	~TextureAtlas();

	glm::vec2 GetAtlasDimensions() const { return m_atlas.GetDimensions(); }
	GLenum GetAtlasTextureSlot() const { return m_atlas.GetSlot(); }
private:
	Texture m_atlas; // Actual OpenGL texture
	
};
}