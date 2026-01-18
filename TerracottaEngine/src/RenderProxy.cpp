#include "RenderProxy.hpp"
#include "spdlog/spdlog.h"
#include "glm/glm.hpp"

namespace TerracottaEngine
{

ChunkRenderProxy::ChunkRenderProxy(uint32_t chunkX, uint32_t chunkY) :
	m_chunkX(chunkX), m_chunkY(chunkY)
{
	m_vertices.reserve(256 * 4); // 16x16 tiles * 4 verts
	m_indices.reserve(256 * 6); // 16x16 tiles * 6 indices
}

ChunkRenderProxy::~ChunkRenderProxy()
{
	// No GPU cleanup needed - manager owns buffers
}

void ChunkRenderProxy::UpdateFromRenderTiles(const RenderTile* tiles, uint32_t tileCount)
{
	m_vertices.clear();
	m_indices.clear();

	SPDLOG_DEBUG("Chunk ({}, {}) updating with {} tiles", m_chunkX, m_chunkY, tileCount);

	// Convert RenderTile[] to Vertex[] and indices
	for (uint32_t i = 0; i < tileCount; ++i) {
		const RenderTile& tile = tiles[i];

		// Quad corners
		glm::vec3 positions[4]
			= {{tile.X, tile.Y, tile.Z}, {tile.X + tile.ScaleX, tile.Y, tile.Z}, {tile.X + tile.ScaleX, tile.Y + tile.ScaleY, tile.Z}, {tile.X, tile.Y + tile.ScaleY, tile.Z}};

		// UV coordinates
		glm::vec2 uvs[4] = {{tile.FrameSlotX, tile.FrameSlotY}, {tile.FrameSlotX + tile.FrameSlotW, tile.FrameSlotY}, {tile.FrameSlotX + tile.FrameSlotW, tile.FrameSlotY + tile.FrameSlotH},
			{tile.FrameSlotX, tile.FrameSlotY + tile.FrameSlotH}};

		// Add 4 vertices
		uint32_t baseIndex = static_cast<uint32_t>(m_vertices.size());
		for (int j = 0; j < 4; ++j) {
			Vertex v;
			v.Position = positions[j];
			v.TextureCoord = uvs[j];
			v.TextureIndex = tile.TextureIndex;
			m_vertices.push_back(v);
		}

		// Add 6 indices (two triangles)
		m_indices.push_back(baseIndex + 0);
		m_indices.push_back(baseIndex + 1);
		m_indices.push_back(baseIndex + 2);
		m_indices.push_back(baseIndex + 2);
		m_indices.push_back(baseIndex + 3);
		m_indices.push_back(baseIndex + 0);
	}

	m_isDirty = true;
	// SPDLOG_INFO("Chunk ({}, {}) has {} vertices, {} indices", m_chunkX, m_chunkY, m_vertices.size(), m_indices.size());
}

ChunkRenderProxyManager::ChunkRenderProxyManager()
{}

ChunkRenderProxyManager::~ChunkRenderProxyManager()
{
	Shutdown();
}

void ChunkRenderProxyManager::InitializeChunks(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks)
{
	m_worldWidthInChunks = worldWidthInChunks;
	m_worldHeightInChunks = worldHeightInChunks;

	uint32_t totalChunks = worldWidthInChunks * worldHeightInChunks;
	m_renderProxies.clear();
	m_renderProxies.reserve(totalChunks);

	for (uint32_t y = 0; y < worldHeightInChunks; y++) {
		for (uint32_t x = 0; x < worldWidthInChunks; x++) {
			m_renderProxies.push_back(std::make_unique<ChunkRenderProxy>(x, y));
		}
	}

	// Create single VAO/VBO/EBO for entire world
	m_vao = std::make_unique<VertexArray>();
	m_vbo = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
	m_ebo = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);

	// Set up vertex attributes
	m_vao->Bind();
	m_vbo->Bind();
	m_vao->LinkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	m_vao->LinkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoord));
	m_vao->LinkAttribute(2, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TextureIndex));
	m_vao->Unbind();

	// Reserve staging buffer space (assume max 256 tiles per chunk)
	uint32_t maxVertices = totalChunks * 256 * 4;
	uint32_t maxIndices = totalChunks * 256 * 6;
	m_vertexBuffer.reserve(maxVertices);
	m_indexBuffer.reserve(maxIndices);

	m_needsRebuild = true;

	SPDLOG_INFO("Initialized {} chunk render proxies ({}x{}) with shared buffers", totalChunks, worldWidthInChunks, worldHeightInChunks);
}

void ChunkRenderProxyManager::Shutdown()
{
	m_renderProxies.clear();
	m_vao.reset();
	m_vbo.reset();
	m_ebo.reset();
	m_vertexBuffer.clear();
	m_indexBuffer.clear();
}

ChunkRenderProxy* ChunkRenderProxyManager::GetChunk(uint32_t chunkX, uint32_t chunkY)
{
	uint32_t index = chunkY * m_worldWidthInChunks + chunkX;
	if (index >= m_renderProxies.size()) {
		SPDLOG_ERROR("Invalid chunk coordinates ({}, {})", chunkX, chunkY);
		return nullptr;
	}
	return m_renderProxies[index].get();
}

void ChunkRenderProxyManager::RebuildBuffers()
{
	m_vertexBuffer.clear();
	m_indexBuffer.clear();

	// Gather all chunk data into staging buffers
	for (auto& chunk : m_renderProxies) {
		const auto& chunkVertices = chunk->GetVertices();
		const auto& chunkIndices = chunk->GetIndices();

		uint32_t vertexOffset = static_cast<uint32_t>(m_vertexBuffer.size());
		uint32_t indexOffset = static_cast<uint32_t>(m_indexBuffer.size());

		// Copy vertices
		m_vertexBuffer.insert(m_vertexBuffer.end(), chunkVertices.begin(), chunkVertices.end());

		// Copy and adjust indices (offset by this chunk's vertex offset)
		for (uint32_t idx : chunkIndices) {
			m_indexBuffer.push_back(idx + vertexOffset);
		}

		// Tell chunk where its data lives in the buffer
		chunk->SetBufferRange(vertexOffset, indexOffset, static_cast<uint32_t>(chunkIndices.size()));
		chunk->ClearDirty();
	}

	// Upload to GPU
	if (!m_vertexBuffer.empty()) {
		m_vbo->Bind();
		m_vbo->BufferInitData(m_vertexBuffer.size() * sizeof(Vertex), m_vertexBuffer.data(), GL_STATIC_DRAW);
	}

	if (!m_indexBuffer.empty()) {
		m_ebo->Bind();
		m_ebo->BufferInitData(m_indexBuffer.size() * sizeof(uint32_t), m_indexBuffer.data(), GL_STATIC_DRAW);
	}

	m_needsRebuild = false;

	SPDLOG_INFO("Rebuilt world buffers: {} vertices, {} indices", m_vertexBuffer.size(), m_indexBuffer.size());
}

void ChunkRenderProxyManager::UploadDirtyChunks()
{
	// Check if any chunks are dirty
	bool anyDirty = false;
	for (auto& chunk : m_renderProxies) {
		if (chunk->IsDirty()) {
			anyDirty = true;
			break;
		}
	}

	// If any chunks changed, rebuild entire buffer
	// TODO: Optimize to only update changed chunks
	if (anyDirty || m_needsRebuild) {
		RebuildBuffers();
	}
}

void ChunkRenderProxyManager::RenderAll()
{
	if (m_indexBuffer.empty())
		return;

	m_vao->Bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.size()), GL_UNSIGNED_INT, 0);
}

} // namespace TerracottaEngine