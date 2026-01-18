#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "VertexInput.hpp"
#include "SharedDataTypes.h"

namespace TerracottaEngine
{

class ChunkRenderProxy
{
public:
	ChunkRenderProxy(uint32_t chunkX, uint32_t chunkY);
	~ChunkRenderProxy();

	// Called when game sends new tile data
	void UpdateFromRenderTiles(const RenderTile* tiles, uint32_t tileCount);

	bool IsDirty() const { return m_isDirty; }
	void ClearDirty() { m_isDirty = false; }

	// Getters for manager to access data
	const std::vector<Vertex>& GetVertices() const { return m_vertices; }
	const std::vector<uint32_t>& GetIndices() const { return m_indices; }
	uint32_t GetChunkX() const { return m_chunkX; }
	uint32_t GetChunkY() const { return m_chunkY; }

	// Set by manager after upload
	void SetBufferRange(uint32_t vertexOffset, uint32_t indexOffset, uint32_t indexCount)
	{
		m_vertexOffset = vertexOffset;
		m_indexOffset = indexOffset;
		m_indexCount = indexCount;
	}

	uint32_t GetVertexOffset() const { return m_vertexOffset; }
	uint32_t GetIndexOffset() const { return m_indexOffset; }
	uint32_t GetIndexCount() const { return m_indexCount; }
private:
	uint32_t m_chunkX, m_chunkY;

	// CPU-side data
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	// Range in the shared buffers (set by manager)
	uint32_t m_vertexOffset = 0;
	uint32_t m_indexOffset = 0;
	uint32_t m_indexCount = 0;

	bool m_isDirty = true;
};

class ChunkRenderProxyManager
{
public:
	ChunkRenderProxyManager();
	~ChunkRenderProxyManager();

	void InitializeChunks(uint32_t worldWidthInChunks, uint32_t worldHeightInChunks);
	void Shutdown();

	ChunkRenderProxy* GetChunk(uint32_t chunkX, uint32_t chunkY);

	// Called by Renderer each frame
	void UploadDirtyChunks();
	void RenderAll();
private:
	// Chunk storage
	std::vector<std::unique_ptr<ChunkRenderProxy>> m_renderProxies;
	uint32_t m_worldWidthInChunks = 0;
	uint32_t m_worldHeightInChunks = 0;

	// For the entire world
	std::unique_ptr<VertexArray> m_vao;
	std::unique_ptr<BufferObject> m_vbo;
	std::unique_ptr<BufferObject> m_ebo;

	// CPU-side staging buffers
	std::vector<Vertex> m_vertexBuffer;
	std::vector<uint32_t> m_indexBuffer;

	bool m_needsRebuild = true;

	void RebuildBuffers();
};

} // namespace TerracottaEngine