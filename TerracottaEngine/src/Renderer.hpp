#pragma once

#include <array>
#include "glm/glm.hpp"
#include "JSONParser.hpp"
#include "Subsystem.hpp"
#include "CameraSystem.hpp"
#include "ShaderProgram.hpp"
#include "VertexInput.hpp"
#include "Textures.hpp"
#include "Window.hpp"

namespace TerracottaEngine
{
struct Renderer2D
{
	std::unique_ptr<ShaderProgram> Shader = nullptr;
	std::unique_ptr<VertexArray> VAO = nullptr;
	std::unique_ptr<BufferObject> VBO = nullptr;
	std::unique_ptr<BufferObject> EBO = nullptr;

	Vertex* VBOBase = nullptr;
	Vertex* VBOPtr = nullptr;
	uint32_t* EBOData = nullptr;

	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	constexpr static uint32_t MAX_QUADS = 10000;
	constexpr static uint32_t MAX_VERTICES = MAX_QUADS * 4;
	constexpr static uint32_t MAX_INDICES = MAX_QUADS * 6;
	constexpr static uint32_t MAX_TEXTURES = 32;
	constexpr static glm::vec4 DEFAULT_QUAD_POSITIONS[4] = {
		{0.0f, 0.0f, 0.0f, 1.0f}, // bottom-left
		{1.0f, 0.0f, 0.0f, 1.0f}, // bottom-right
		{1.0f, 1.0f, 0.0f, 1.0f}, // top-right
		{0.0f, 1.0f, 0.0f, 1.0f} // top-left
	};
	constexpr static glm::vec2 DEFAULT_QUAD_TEXCOORDS[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	
	std::array<TextureAtlas*, MAX_TEXTURES> AtlasSlots; // In Use
	std::vector<std::unique_ptr<TextureAtlas>> Atlases; // In Storage
	std::array<std::unique_ptr<Texture>, MAX_TEXTURES> TextureSlots;
	uint32_t TextureSlotIndex = 0;
};

class Renderer : public Subsystem
{
public:
	Renderer(SubsystemManager& manager, Window& appWindow);
	~Renderer();

	virtual bool Init() override;
	virtual void OnUpdate(const float deltaTime) override;
	virtual void Shutdown() override;
	
	void BeginBatch();
	void EndBatch();
	void Flush();
	void OnRender();

	// Tilemap stuff
	void DrawTilemapData(const TilemapData& tilemap);
	void DrawTilemapQuad(int tileX, int tileY, int tileId, TextureAtlas* atlas, uint32_t atlasSlot);
	uint32_t AddTextureAtlas(TextureAtlas* atlas);

	// Debug shader = 2D colors
	void DrawQuad(const glm::vec3& position3D, float theta = 0.0f, const glm::vec2& scale = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float index = 0.0f);
	void DrawQuad(const glm::mat4& transform, const glm::vec4& color, float index);
private:
	// Make sure this updated if necessary!
	Window* m_appWindow = nullptr;
	Camera m_camera;

	// Separate 2D (and potentially 3D renderer)
	Renderer2D m_renderer2D;
	JSONParser m_parser;

	bool is2DVBOFull(uint32_t addVertex) const { return m_renderer2D.VertexCount + addVertex > Renderer2D::MAX_VERTICES; }
	bool is2DTexturesFull() const { return m_renderer2D.TextureSlotIndex >= Renderer2D::MAX_TEXTURES; }
};
}