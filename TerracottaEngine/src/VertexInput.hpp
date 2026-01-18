#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"

// Contains VAO, VBO, EBO, etc.
namespace TerracottaEngine
{
struct Vertex
{
	glm::vec3 Position; // X, Y, Z
	glm::vec2 TextureCoord; // U, V
	float TextureIndex; // Texture slot
};

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const { glBindVertexArray(m_id); }
	void Unbind() const { glBindVertexArray(0); }
	void LinkAttribute(GLuint layoutIndex, GLuint size, GLenum type, GLsizei stride, const void* offset);
	void LinkInstanceAttribute(GLuint layoutIndex, GLuint size, GLenum type, GLsizei stride, const void* offset);
private:
	GLuint m_id = 0;
};

class BufferObject
{
public:
	BufferObject(GLenum type);
	~BufferObject();

	void Bind() const { glBindBuffer(m_type, m_id); }
	void BufferInitData(GLsizeiptr size, const void* data, GLenum usage);
	void BufferSubData(GLintptr offset, GLsizeiptr size, const void* data);
private:
	GLuint m_id = 0;
	GLenum m_type;
};
} // namespace TerracottaEngine