#pragma once

#include "glm/glm.hpp"
#include "Subsystem.hpp"
#include "CameraSystem.hpp"
#include "ShaderProgram.hpp"
#include "VertexInput.hpp"
#include "Textures.hpp"
#include "Window.hpp"

namespace TerracottaEngine
{
class Renderer : public Subsystem
{
public:
	Renderer(SubsystemManager& manager, Window& appWindow);
	~Renderer();

	virtual bool Init() override;
	virtual void OnUpdate(const float deltaTime) override;
	virtual void Shutdown() override;

	void OnProcessInput();
	void OnRender();
private:
	// Make sure this updated if necessary!
	Window& m_appWindow;
	Camera m_camera;

	std::unique_ptr<ShaderProgram> m_shader = nullptr;
	std::unique_ptr<Texture> m_texture = nullptr;
	std::unique_ptr<VertexArray> m_vao = nullptr;
	std::unique_ptr<BufferObject> m_vbo = nullptr;
	std::unique_ptr<BufferObject> m_ebo = nullptr;
};
}