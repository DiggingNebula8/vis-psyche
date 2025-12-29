#include "Mesh.h"

namespace VizEngine
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	{
		SetupMesh(
			reinterpret_cast<const float*>(vertices.data()),
			vertices.size() * sizeof(Vertex),
			indices.data(),
			indices.size()
		);
	}

	Mesh::Mesh(const float* vertexData, size_t vertexDataSize, const unsigned int* indices, size_t indexCount)
	{
		SetupMesh(vertexData, vertexDataSize, indices, indexCount);
	}

	void Mesh::SetupMesh(const float* vertexData, size_t vertexDataSize, const unsigned int* indices, size_t indexCount)
	{
		m_VertexArray = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertexData, static_cast<unsigned int>(vertexDataSize));

		VertexBufferLayout layout;
		layout.Push<float>(4); // Position (vec4)
		layout.Push<float>(4); // Color (vec4)
		layout.Push<float>(2); // TexCoords (vec2)

		m_VertexArray->LinkVertexBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, static_cast<unsigned int>(indexCount));
	}

	void Mesh::Bind() const
	{
		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
	}

	void Mesh::Unbind() const
	{
		m_VertexArray->Unbind();
		m_IndexBuffer->Unbind();
	}

	std::unique_ptr<Mesh> Mesh::CreatePyramid()
	{
		std::vector<Vertex> vertices = {
			// Base of the Pyramid
			Vertex(glm::vec4(-0.5f, 0.0f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 2.0f)),
			Vertex(glm::vec4( 0.5f, 0.0f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(2.0f, 2.0f)),
			Vertex(glm::vec4( 0.5f, 0.0f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(2.0f, 0.0f)),
			// Tip of the Pyramid
			Vertex(glm::vec4( 0.0f, 0.8f,  0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f))
		};

		std::vector<unsigned int> indices = {
			0, 1, 2,   // base
			0, 2, 3,
			0, 1, 4,   // sides
			1, 2, 4,
			2, 3, 4,
			3, 0, 4
		};

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Mesh::CreateCube()
	{
		std::vector<Vertex> vertices = {
			// Front face
			Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
			// Back face
			Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		};

		std::vector<unsigned int> indices = {
			// Front
			0, 1, 2, 2, 3, 0,
			// Back
			5, 4, 7, 7, 6, 5,
			// Left
			4, 0, 3, 3, 7, 4,
			// Right
			1, 5, 6, 6, 2, 1,
			// Top
			3, 2, 6, 6, 7, 3,
			// Bottom
			4, 5, 1, 1, 0, 4
		};

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Mesh::CreatePlane(float size)
	{
		float half = size / 2.0f;
		std::vector<Vertex> vertices = {
			Vertex(glm::vec4(-half, 0.0f, -half, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec4( half, 0.0f, -half, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec4( half, 0.0f,  half, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec4(-half, 0.0f,  half, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		};

		std::vector<unsigned int> indices = {
			0, 1, 2,
			2, 3, 0
		};

		return std::make_unique<Mesh>(vertices, indices);
	}
}

