#pragma once

#include <filesystem>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace TerracottaEngine
{
class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram() { glDeleteProgram(m_id); }

	void InitializeShaderProgram(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);

	void UploadUniformInt(const std::string& uniformName, GLint value);
	void UploadUniformIntArray(const std::string& uniformName, GLsizei count, const GLint* value);
	void UploadUniformMat4(const std::string& uniformName, const glm::mat4& matrix);

	void Use() const { glUseProgram(m_id); }
	void Deactivate() const { glUseProgram(0); }
	GLuint GetID() const { return m_id; }
private:
	GLuint m_id = 0;
	GLuint compileShader(GLuint type, const std::filesystem::path& shader);
};
} // namespace TerracottaEngine