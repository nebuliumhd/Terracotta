#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.hpp"

namespace TerracottaEngine
{
Renderer::Renderer(SubsystemManager& subsystemManager, Window& appWindow) :
	Subsystem(subsystemManager), m_appWindow(&appWindow), m_camera(subsystemManager, appWindow)
{}
Renderer::~Renderer()
{}

bool Renderer::Init()
{
	SPDLOG_INFO("Initializing renderer...");

	if (!m_appWindow) {
		SPDLOG_ERROR("There is no Window attached to the Renderer.");
		return false;
	}

	// Basic OpenGL config
	glm::ivec2 windowSize = m_appWindow->GetWindowSize();
	glViewport(0, 0, windowSize.x, windowSize.y);
	glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Init Renderer2D
	m_renderer2D.VBOBase = new Vertex[Renderer2D::MAX_VERTICES]; // Use std::unique_ptr?
	m_renderer2D.VBOPtr = &m_renderer2D.VBOBase[0];
	m_renderer2D.EBOData = new uint32_t[Renderer2D::MAX_INDICES];
	uint32_t offset = 0;
	for (uint32_t i = 0; i < Renderer2D::MAX_INDICES; i += 6) {
		m_renderer2D.EBOData[i] = offset;
		m_renderer2D.EBOData[i + 1] = offset + 1;
		m_renderer2D.EBOData[i + 2] = offset + 2;
		m_renderer2D.EBOData[i + 3] = offset + 2;
		m_renderer2D.EBOData[i + 4] = offset + 3;
		m_renderer2D.EBOData[i + 5] = offset;
		offset += 4;
	}

	m_renderer2D.Shader = std::make_unique<ShaderProgram>();
	m_renderer2D.Shader->InitializeShaderProgram("../../../../../TerracottaEngine/res/DefaultVert.glsl", "../../../../../TerracottaEngine/res/DefaultFrag.glsl");

	// For vertex shader
	m_renderer2D.Shader->UploadUniformMat4("u_view", m_camera.View);
	m_renderer2D.Shader->UploadUniformMat4("u_projection", m_camera.Projection);

	// For fragment shader
	int samplers[Renderer2D::MAX_TEXTURES];
	for (int i = 0; i < Renderer2D::MAX_TEXTURES; i++) {
		samplers[i] = i;
	}
	m_renderer2D.Shader->UploadUniformIntArray("u_textures", Renderer2D::MAX_TEXTURES, samplers);

	m_renderer2D.TextureSlotIndex = 0;
	m_renderer2D.TextureSlots[0] = std::make_unique<Texture>("../../../../../TerracottaEngine/res/DebugTexture.jpg");
	glActiveTexture(GL_TEXTURE0);
	m_renderer2D.TextureSlots[0]->Bind();
	m_renderer2D.TextureSlotIndex = 1;

	m_renderer2D.VAO = std::make_unique<VertexArray>();
	m_renderer2D.VBO = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);

	TilemapData data = m_parser.LoadTilemapFromFile("C:\\Users\\garla\\source\\repos\\Terracotta\\TerracottaGame\\res\\maps\\default_map\\DefaultMap.json");
	DrawTilemapData(data);
	m_renderer2D.VBO->BufferInitData(Renderer2D::MAX_VERTICES * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
	m_renderer2D.VBO->BufferSubData(0, m_renderer2D.VertexCount * sizeof(Vertex), m_renderer2D.VBOBase);
	m_renderer2D.EBO = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);
	m_renderer2D.EBO->BufferInitData(Renderer2D::MAX_INDICES * sizeof(uint32_t), m_renderer2D.EBOData, GL_STATIC_DRAW);

	m_renderer2D.VAO->LinkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // X, Y, Z
	m_renderer2D.VAO->LinkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // U, V
	m_renderer2D.VAO->LinkAttribute(2, 1, GL_FLOAT, sizeof(Vertex), (void*)(5 * sizeof(GLfloat))); // Texture Index
	m_renderer2D.VAO->Unbind();
	m_renderer2D.Shader->Deactivate();

	SPDLOG_INFO("Finished initializing renderer.");

	return true;
}
void Renderer::Shutdown()
{
	delete[] m_renderer2D.VBOBase;
	m_renderer2D.VBOBase = nullptr;
	delete[] m_renderer2D.EBOData;
	m_renderer2D.EBOData = nullptr;
}

void Renderer::OnUpdate(const float deltaTime)
{
	m_camera.Update(deltaTime);

	if (m_camera.NeedsUpdate) {
		m_renderer2D.Shader->Use();
		m_renderer2D.Shader->UploadUniformMat4("u_view", m_camera.View);
		m_renderer2D.Shader->UploadUniformMat4("u_projection", m_camera.Projection);

		m_camera.NeedsUpdate = false;
	}
}
void Renderer::BeginBatch()
{
	// Resets VBOPtr back to VBOBase; Resets VertexCount and IndexCount to 0; Clears texture slot tracking
	m_renderer2D.VBOPtr = &m_renderer2D.VBOBase[0];
	m_renderer2D.VertexCount = 0;
	m_renderer2D.IndexCount = 0;
	m_renderer2D.TextureSlotIndex = 1;

	for (uint32_t i = 1; i < Renderer2D::MAX_TEXTURES; i++) {
		m_renderer2D.AtlasSlots[i] = nullptr;
	}
}
void Renderer::EndBatch()
{
	//  Uploads the accumulated vertex/index data to GPU buffers (glBufferSubData); Optionally calls Flush() to actually draw
	m_renderer2D.VBO->BufferSubData(0, m_renderer2D.VertexCount * sizeof(Vertex), m_renderer2D.VBOBase);
	Flush();
}
void Renderer::Flush()
{
	// Binds all textures to their slots; Calls glDrawElements() or glDrawArrays(); Reset the batch if needed
	if (m_renderer2D.VertexCount == 0) // Nothing to draw
		return;

	for (uint32_t i = 1; i < m_renderer2D.TextureSlotIndex; i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		if (m_renderer2D.AtlasSlots[i]) {
			m_renderer2D.AtlasSlots[i]->GetTexture().Bind();
		} else if (m_renderer2D.TextureSlots[i]) {
			m_renderer2D.TextureSlots[i]->Bind();
		}
	}

	m_renderer2D.Shader->Use();
	m_renderer2D.VAO->Bind();

	glDrawElements(GL_TRIANGLES, m_renderer2D.IndexCount, GL_UNSIGNED_INT, 0);
}
void Renderer::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_renderer2D.IndexCount > 0)
		Flush();

	// Application will take care of swapping buffers
}

void Renderer::DrawTilemapData(const TilemapData& tilemap)
{
	uint32_t requiredVertices = static_cast<uint32_t>(tilemap.Tiles.size()) * 4;
	if (m_renderer2D.VertexCount + requiredVertices > Renderer2D::MAX_VERTICES) {
		SPDLOG_ERROR("Buffer too small! Need {} vertices, have {}", requiredVertices, Renderer2D::MAX_VERTICES - m_renderer2D.VertexCount);
		return;
	}

	std::unique_ptr<TextureAtlas> atlas = std::make_unique<TextureAtlas>(tilemap.AtlasPath, tilemap.AtlasRows, tilemap.AtlasColumns);
	m_renderer2D.Atlases.push_back(std::move(atlas));

	TextureAtlas* atlasPtr = m_renderer2D.Atlases.back().get();
	uint32_t atlasSlot = AddTextureAtlas(atlasPtr);

	int tileIndex = 0;
	for (int tileY = 0; tileY < tilemap.Height; tileY++) {
		for (int tileX = 0; tileX < tilemap.Width; tileX++) {
			int tileId = tilemap.Tiles[tileIndex++];
			DrawTilemapQuad(tileX, tileY, tileId, atlasPtr, atlasSlot);
		}
	}

	SPDLOG_INFO("Loaded tilemap \"{}\" into renderer: {} vertices", tilemap.Name, requiredVertices);
}

void Renderer::DrawTilemapQuad(int tileX, int tileY, int tileId, TextureAtlas* atlas, uint32_t atlasSlot)
{
	glm::vec4 uvs = atlas->GetTileUVs(tileId);
	glm::vec2 vertexUVs[4] = {{uvs.x, uvs.y}, {uvs.z, uvs.y}, {uvs.z, uvs.w}, {uvs.x, uvs.w}};
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tileX, tileY, 0.0f));
	for (size_t i = 0; i < 4; i++) {
		m_renderer2D.VBOPtr->Position = transform * Renderer2D::DEFAULT_QUAD_POSITIONS[i];
		m_renderer2D.VBOPtr->TextureCoord = vertexUVs[i];
		m_renderer2D.VBOPtr->TextureIndex = static_cast<float>(atlasSlot);
		m_renderer2D.VBOPtr++;
	}

	m_renderer2D.VertexCount += 4;
	m_renderer2D.IndexCount += 6;
}

uint32_t Renderer::AddTextureAtlas(TextureAtlas* atlas)
{
	if (!atlas)
		return 0;

	// If the texture atlas already exists, just simply return it
	for (uint32_t i = 0; i < m_renderer2D.TextureSlotIndex; i++) {
		if (m_renderer2D.AtlasSlots[i] == atlas) {
			return i;
		}
	}

	if (m_renderer2D.TextureSlotIndex >= Renderer2D::MAX_TEXTURES) {
		SPDLOG_WARN("Texture slots full!");
		return 0;
	}

	// Otherwise increment hte global texture slot index
	uint32_t slot = m_renderer2D.TextureSlotIndex++;
	m_renderer2D.AtlasSlots[slot] = atlas;
	return slot;
}

void Renderer::DrawQuad(const glm::vec3& position3D, float theta, const glm::vec2& scale, const glm::vec4& color, float index)
{
	if (is2DVBOFull(4) || is2DTexturesFull()) {
		EndBatch(); // Upload & render
		BeginBatch();
	}

	glm::mat4 transform = glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f}); // Scale
	transform = glm::rotate(transform, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
	transform = glm::translate(transform, position3D); // Translate
	DrawQuad(transform, color, index);
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, float index)
{
	// Complete vertex data
	for (size_t i = 0; i < 4; i++) {
		m_renderer2D.VBOPtr->Position = transform * Renderer2D::DEFAULT_QUAD_POSITIONS[i];
		m_renderer2D.VBOPtr->TextureCoord = Renderer2D::DEFAULT_QUAD_TEXCOORDS[i];
		m_renderer2D.VBOPtr->TextureIndex = index;
		m_renderer2D.VBOPtr++;
	}

	m_renderer2D.VertexCount += 4;
	m_renderer2D.IndexCount += 6;
}
}