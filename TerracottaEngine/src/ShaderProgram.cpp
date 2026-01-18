#include <fstream>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "spdlog/spdlog.h"
#include "ShaderProgram.hpp"

namespace TerracottaEngine
{
ShaderProgram::ShaderProgram()
{
	m_id = glCreateProgram();
}

void ShaderProgram::InitializeShaderProgram(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader)
{
	GLuint vShaderID = compileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fShaderID = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach vertex and fragment shader to graphics pipeline
	glAttachShader(m_id, vShaderID);
	glAttachShader(m_id, fShaderID);
	glLinkProgram(m_id);

	GLint linkStatus, infoLogLength;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);
	glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (linkStatus == GL_FALSE) {
		std::vector<GLchar> linkErrMsg(infoLogLength);
		glGetProgramInfoLog(m_id, infoLogLength, &infoLogLength, linkErrMsg.data());
		SPDLOG_ERROR("\"{}\" and \"{}\" have failed to link: {}", vertexShader.filename().string(), fragmentShader.filename().string(), linkErrMsg.data());
	} else {
		SPDLOG_INFO("\"{}\" and \"{}\" have linked successfully!", vertexShader.filename().string(), fragmentShader.filename().string());
	}

	// Clean up
	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);
	
	// Can only use the program AFTER compile/link is successful.
	glUseProgram(m_id);
}

GLuint ShaderProgram::compileShader(GLuint type, const std::filesystem::path& shader)
{
	// Ensure path is valid
	if (!std::filesystem::exists(shader)) {
		SPDLOG_ERROR("There is no shader file with the name \"{}\" found in \"{}\"", shader.filename().string(), shader.parent_path().string());
		return 0;
	}

	GLuint shaderID;
	switch (type) {
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:
		shaderID = glCreateShader(type);
		break;
	default:
		SPDLOG_ERROR("Invalid shader type with the value {} entered!", type);
		return 0;
	}

	std::string shaderCode;
	std::ifstream shaderFileStream(shader, std::ios_base::in);
	if (shaderFileStream.is_open()) {
		std::stringstream sstr;
		sstr << shaderFileStream.rdbuf();
		shaderCode = sstr.str();
		shaderFileStream.close();
	} else {
		SPDLOG_ERROR("Could not open the shader file \"{}\" found in \"{}\"", shader.filename().string(), shader.parent_path().string());
		return 0;
	}

	const char* shaderFileContents = shaderCode.c_str();
	glShaderSource(shaderID, 1, &shaderFileContents, nullptr);
	glCompileShader(shaderID);

	// Check compilation status afterwards
	GLint shaderStatus, infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderStatus);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (shaderStatus == GL_FALSE) {
		std::vector<GLchar> compErrMsg(infoLogLength);
		glGetProgramInfoLog(m_id, infoLogLength, &infoLogLength, compErrMsg.data());
		SPDLOG_ERROR("\"{}\" has failed to compile: {}", shader.filename().string(), compErrMsg.data());
	} else {
		SPDLOG_INFO("\"{}\" has compiled successfully!", shader.filename().string());
	}

	return shaderID;
}

void ShaderProgram::UploadUniformInt(const std::string& uniformName, GLint value)
{
	GLint location = glGetUniformLocation(m_id, uniformName.c_str());
	if (location == -1) {
		SPDLOG_ERROR("There is no int uniform called \"{}\" in the shader program.", uniformName);
	}
	glUniform1i(location, value);
}
void ShaderProgram::UploadUniformIntArray(const std::string& uniformName, GLsizei count, const GLint* value)
{
	GLint location = glGetUniformLocation(m_id, uniformName.c_str());
	if (location == -1) {
		SPDLOG_ERROR("There is no int uniform called \"{}\" in the shader program.", uniformName);
	}
	glUniform1iv(location, count, value);
}
void ShaderProgram::UploadUniformMat4(const std::string& uniformName, const glm::mat4& matrix)
{
	GLint location = glGetUniformLocation(m_id, uniformName.c_str());
	if (location == -1) {
		SPDLOG_ERROR("There is no mat4 uniform called \"{}\" in the shader program.", uniformName);
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
} // namespace TerracottaEngine