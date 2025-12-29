#pragma once

#include "VizEngine/Core.h"
#include "glm.hpp"
#include "VizEngine/OpenGL/VertexArray.h"
#include "VizEngine/OpenGL/VertexBuffer.h"
#include "VizEngine/OpenGL/IndexBuffer.h"
#include "VizEngine/OpenGL/VertexBufferLayout.h"
#include <vector>
#include <memory>

namespace VizEngine
{
	// Vertex structure with position, color, and texture coordinates
	struct Vertex
	{
		glm::vec4 Position;
		glm::vec4 Color;
		glm::vec2 TexCoords;

		Vertex() = default;
		Vertex(const glm::vec4& pos, const glm::vec4& col, const glm::vec2& tex)
			: Position(pos), Color(col), TexCoords(tex) {}
	};

	class VizEngine_API Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
		Mesh(const float* vertexData, size_t vertexDataSize, const unsigned int* indices, size_t indexCount);
		~Mesh() = default;

		// Prevent copying
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		// Allow moving
		Mesh(Mesh&& other) noexcept = default;
		Mesh& operator=(Mesh&& other) noexcept = default;

		void Bind() const;
		void Unbind() const;

		unsigned int GetIndexCount() const { return m_IndexBuffer->GetCount(); }
		const VertexArray& GetVertexArray() const { return *m_VertexArray; }
		const IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }

		// Factory methods for common shapes
		static std::unique_ptr<Mesh> CreatePyramid();
		static std::unique_ptr<Mesh> CreateCube();
		static std::unique_ptr<Mesh> CreatePlane(float size = 1.0f);

	private:
		void SetupMesh(const float* vertexData, size_t vertexDataSize, const unsigned int* indices, size_t indexCount);

		std::unique_ptr<VertexArray> m_VertexArray;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};
}

