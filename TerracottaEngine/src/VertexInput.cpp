#include "VertexInput.hpp"

namespace TerracottaEngine
{
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);
}
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}

void VertexArray::LinkAttribute(GLuint layoutIndex, GLuint size, GLenum type, GLsizei stride, const void* offset)
{
	glVertexAttribPointer(layoutIndex, size, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layoutIndex);
}
void VertexArray::LinkInstanceAttribute(GLuint layoutIndex, GLuint size, GLenum type, GLsizei stride, const void* offset)
{
	LinkAttribute(layoutIndex, size, type, stride, offset);
	glVertexAttribDivisor(layoutIndex, 1);
}



BufferObject::BufferObject(GLenum type)
{
	glGenBuffers(1, &m_id);
	m_type = type;
	glBindBuffer(m_type, m_id);
}
BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &m_id);
}

void BufferObject::BufferInitData(GLsizeiptr size, const void* data, GLenum usage)
{
	glBufferData(m_type, size, data, usage);
}
void BufferObject::BufferSubData(GLintptr offset, GLsizeiptr size, const void* data)
{
	glBufferSubData(m_type, offset, size, data);
}

} // namespace TerracottaEngine