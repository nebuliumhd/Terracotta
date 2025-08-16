#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include "Renderer.hpp"

namespace TerracottaEngine
{
Renderer::Renderer(SubsystemManager& subsystemManager, Window& appWindow) :
	Subsystem(subsystemManager), m_appWindow(appWindow), m_camera(subsystemManager)
{
	SPDLOG_INFO("Initializing renderer...");

	glm::ivec2 windowSize = appWindow.GetWindowSize();
	glViewport(0, 0, windowSize.x, windowSize.y);
	glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_shader = std::make_unique<ShaderProgram>();
	m_shader->InitializeShaderProgram("../../../../TerracottaEngine/res/DefaultVert.glsl", "../../../../TerracottaEngine/res/DefaultFrag.glsl");
	m_shader->UploadUniformMat4("u_view", m_camera.View);
	m_shader->UploadUniformMat4("u_projection", m_camera.Projection);

	m_texture = std::make_unique<Texture>("../../../../TerracottaEngine/res/DefaultTexture.jpg");
	m_texture->Activate();
	m_texture->Bind();
	m_shader->UploadUniformInt("u_texture", 0 /*For GL_TEXTURE0*/);

	m_vao = std::make_unique<VertexArray>();
	
	const GLfloat triangleData[] = {0.5f, 0.5f, 0.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};
	const GLuint indexData[] = {0, 1, 2, 2, 3, 0};
	m_vbo = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
	m_vbo->BufferData(sizeof(triangleData), triangleData, GL_STATIC_DRAW);

	// Instance buffer
	m_ebo = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);
	m_ebo->BufferData(sizeof(indexData), indexData, GL_STATIC_DRAW);

	m_vao->LinkAttribute(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0); // X, Y, Z
	m_vao->LinkAttribute(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // R, G, B

	m_vao->Unbind();
	m_shader->Deactivate();

	SPDLOG_INFO("Finished initializing renderer.");
}
Renderer::~Renderer()
{

}

bool Renderer::Init()
{
	return true;
}
void Renderer::Shutdown()
{

}

void Renderer::OnProcessInput()
{
	m_camera.ProcessInput();
}
void Renderer::OnUpdate(const float deltaTime)
{
	if (m_camera.NeedsUpdate) {
		m_camera.Update(deltaTime);

		m_shader->Use();
		m_shader->UploadUniformMat4("u_view", m_camera.View);
		m_shader->UploadUniformMat4("u_projection", m_camera.Projection);
		
		m_camera.NeedsUpdate = false;
	}
}
void Renderer::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT);

	m_shader->Use();
	m_vao->Bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Application will take care of swapping buffers
}
}