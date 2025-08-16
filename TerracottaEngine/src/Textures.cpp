#include "spdlog/spdlog.h"
#include "stb/stb_image.h"
#include "Textures.hpp"

namespace TerracottaEngine
{
Texture::Texture(const std::string& texturePath) :
	m_dimensions(glm::vec2(0.0f))
{
	if (!std::filesystem::exists(texturePath)) {
		SPDLOG_INFO("The texture file {} does not exist.", texturePath);
		return;
	}

	int imgWidth, imgHeight, numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imgData = stbi_load(texturePath.c_str(), &imgWidth, &imgHeight, &numChannels, 0);

	if (!imgData) {
		SPDLOG_ERROR("Failed to load texture file at \"{}\".", texturePath);
		return;
	}

	m_dimensions = glm::vec2((float)imgWidth, (float)imgHeight);
	SPDLOG_INFO("Loaded texture file at {} of {{{} x {}}} with {} color channels.", texturePath, m_dimensions.x, m_dimensions.y, numChannels);

	// Upload image to OpenGL and free resources
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLenum format = (numChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imgData);
}
Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

TextureAtlas::TextureAtlas(const std::string& atlas) :
	m_atlas(atlas)
{

}
TextureAtlas::~TextureAtlas()
{

}
}