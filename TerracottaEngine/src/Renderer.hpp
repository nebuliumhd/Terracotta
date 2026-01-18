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
#include "RenderProxy.hpp"
#include "SharedDataTypes.h"

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

	std::array<TextureAtlas*, MAX_TEXTURES> AtlasSlots;
	std::vector<std::unique_ptr<TextureAtlas>> Atlases;
	std::array<std::unique_ptr<Texture>, MAX_TEXTURES> TextureSlots = {nullptr};
	uint32_t TextureSlotIndex = 0;

	// Chunk management
	ChunkRenderProxyManager ChunkManager;
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

	// Game API
	void InitChunkProxies(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks);
	void UpdateChunkTiles(uint32_t chunkX, uint32_t chunkY, const RenderTile* tiles, uint32_t tileCount);
	uint32_t LoadAndAddTextureAtlas(const char* path);
	int GetAtlasInfo(uint32_t atlasId, AtlasInfo* outInfo);
	void GetTileUVs(uint32_t atlasId, uint32_t tileId, UVData* outData);

	// Legacy/Debug
	void DrawTilemapData(const TilemapData& tilemap);
	void DrawTilemapQuad(int tileX, int tileY, int tileId, TextureAtlas* atlas, uint32_t atlasSlot);
	uint32_t AddTextureAtlas(TextureAtlas* atlas);
	void DrawQuad(const glm::vec3& position3D, float theta = 0.0f, const glm::vec2& scale = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float index = 0.0f);
	void DrawQuad(const glm::mat4& transform, const glm::vec4& color, float index);
private:
	Window* m_appWindow = nullptr;
	Camera m_camera;
	Renderer2D m_renderer2D;

	bool is2DVBOFull(uint32_t addVertex) const { return m_renderer2D.VertexCount + addVertex > Renderer2D::MAX_VERTICES; }
	bool is2DTexturesFull() const { return m_renderer2D.TextureSlotIndex >= Renderer2D::MAX_TEXTURES; }
};
} // namespace TerracottaEngine