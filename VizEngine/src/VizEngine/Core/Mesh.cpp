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
		layout.Push<float>(3); // Normal (vec3)
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
		// For proper lighting, each face needs its own vertices with correct normals
		glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		
		// Calculate face normals for the 4 side faces
		// Side face normals point outward at an angle
		float sideAngle = glm::atan(0.8f, 0.5f); // Height / half-base
		float ny = glm::sin(glm::radians(90.0f) - sideAngle);
		float nxz = glm::cos(glm::radians(90.0f) - sideAngle);
		
		glm::vec3 normFront(0.0f, ny, nxz);         // Front face (+Z)
		glm::vec3 normBack(0.0f, ny, -nxz);         // Back face (-Z)
		glm::vec3 normLeft(-nxz, ny, 0.0f);         // Left face (-X)
		glm::vec3 normRight(nxz, ny, 0.0f);         // Right face (+X)
		glm::vec3 normDown(0.0f, -1.0f, 0.0f);      // Base (down)

		std::vector<Vertex> vertices = {
			// Base (facing down) - 4 vertices
			Vertex(glm::vec4(-0.5f, 0.0f,  0.5f, 1.0f), normDown, white, glm::vec2(0.0f, 0.0f)),  // 0
			Vertex(glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f), normDown, white, glm::vec2(0.0f, 1.0f)),  // 1
			Vertex(glm::vec4( 0.5f, 0.0f, -0.5f, 1.0f), normDown, white, glm::vec2(1.0f, 1.0f)),  // 2
			Vertex(glm::vec4( 0.5f, 0.0f,  0.5f, 1.0f), normDown, white, glm::vec2(1.0f, 0.0f)),  // 3

			// Front face (+Z side) - 3 vertices
			Vertex(glm::vec4(-0.5f, 0.0f,  0.5f, 1.0f), normFront, white, glm::vec2(0.0f, 0.0f)), // 4
			Vertex(glm::vec4( 0.5f, 0.0f,  0.5f, 1.0f), normFront, white, glm::vec2(1.0f, 0.0f)), // 5
			Vertex(glm::vec4( 0.0f, 0.8f,  0.0f, 1.0f), normFront, white, glm::vec2(0.5f, 1.0f)), // 6

			// Right face (+X side) - 3 vertices
			Vertex(glm::vec4( 0.5f, 0.0f,  0.5f, 1.0f), normRight, white, glm::vec2(0.0f, 0.0f)), // 7
			Vertex(glm::vec4( 0.5f, 0.0f, -0.5f, 1.0f), normRight, white, glm::vec2(1.0f, 0.0f)), // 8
			Vertex(glm::vec4( 0.0f, 0.8f,  0.0f, 1.0f), normRight, white, glm::vec2(0.5f, 1.0f)), // 9

			// Back face (-Z side) - 3 vertices
			Vertex(glm::vec4( 0.5f, 0.0f, -0.5f, 1.0f), normBack, white, glm::vec2(0.0f, 0.0f)),  // 10
			Vertex(glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f), normBack, white, glm::vec2(1.0f, 0.0f)),  // 11
			Vertex(glm::vec4( 0.0f, 0.8f,  0.0f, 1.0f), normBack, white, glm::vec2(0.5f, 1.0f)),  // 12

			// Left face (-X side) - 3 vertices
			Vertex(glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f), normLeft, white, glm::vec2(0.0f, 0.0f)),  // 13
			Vertex(glm::vec4(-0.5f, 0.0f,  0.5f, 1.0f), normLeft, white, glm::vec2(1.0f, 0.0f)),  // 14
			Vertex(glm::vec4( 0.0f, 0.8f,  0.0f, 1.0f), normLeft, white, glm::vec2(0.5f, 1.0f)),  // 15
		};

		std::vector<unsigned int> indices = {
			// Base
			0, 2, 1,
			0, 3, 2,
			// Front
			4, 5, 6,
			// Right
			7, 8, 9,
			// Back
			10, 11, 12,
			// Left
			13, 14, 15
		};

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Mesh::CreateCube()
	{
		// For proper lighting, each face needs its own vertices with correct normals
		glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		
		// Face normals
		glm::vec3 normFront(0.0f, 0.0f, 1.0f);
		glm::vec3 normBack(0.0f, 0.0f, -1.0f);
		glm::vec3 normLeft(-1.0f, 0.0f, 0.0f);
		glm::vec3 normRight(1.0f, 0.0f, 0.0f);
		glm::vec3 normTop(0.0f, 1.0f, 0.0f);
		glm::vec3 normBottom(0.0f, -1.0f, 0.0f);

		std::vector<Vertex> vertices = {
			// Front face (+Z)
			Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), normFront, white, glm::vec2(0.0f, 0.0f)),  // 0
			Vertex(glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), normFront, white, glm::vec2(1.0f, 0.0f)),  // 1
			Vertex(glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), normFront, white, glm::vec2(1.0f, 1.0f)),  // 2
			Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), normFront, white, glm::vec2(0.0f, 1.0f)),  // 3

			// Back face (-Z)
			Vertex(glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), normBack, white, glm::vec2(0.0f, 0.0f)),   // 4
			Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), normBack, white, glm::vec2(1.0f, 0.0f)),   // 5
			Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), normBack, white, glm::vec2(1.0f, 1.0f)),   // 6
			Vertex(glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), normBack, white, glm::vec2(0.0f, 1.0f)),   // 7

			// Left face (-X)
			Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), normLeft, white, glm::vec2(0.0f, 0.0f)),   // 8
			Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), normLeft, white, glm::vec2(1.0f, 0.0f)),   // 9
			Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), normLeft, white, glm::vec2(1.0f, 1.0f)),   // 10
			Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), normLeft, white, glm::vec2(0.0f, 1.0f)),   // 11

			// Right face (+X)
			Vertex(glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), normRight, white, glm::vec2(0.0f, 0.0f)),  // 12
			Vertex(glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), normRight, white, glm::vec2(1.0f, 0.0f)),  // 13
			Vertex(glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), normRight, white, glm::vec2(1.0f, 1.0f)),  // 14
			Vertex(glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), normRight, white, glm::vec2(0.0f, 1.0f)),  // 15

			// Top face (+Y)
			Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), normTop, white, glm::vec2(0.0f, 0.0f)),    // 16
			Vertex(glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), normTop, white, glm::vec2(1.0f, 0.0f)),    // 17
			Vertex(glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), normTop, white, glm::vec2(1.0f, 1.0f)),    // 18
			Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), normTop, white, glm::vec2(0.0f, 1.0f)),    // 19

			// Bottom face (-Y)
			Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), normBottom, white, glm::vec2(0.0f, 0.0f)), // 20
			Vertex(glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), normBottom, white, glm::vec2(1.0f, 0.0f)), // 21
			Vertex(glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), normBottom, white, glm::vec2(1.0f, 1.0f)), // 22
			Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), normBottom, white, glm::vec2(0.0f, 1.0f)), // 23
		};

		std::vector<unsigned int> indices = {
			// Front
			0, 1, 2, 2, 3, 0,
			// Back
			4, 5, 6, 6, 7, 4,
			// Left
			8, 9, 10, 10, 11, 8,
			// Right
			12, 13, 14, 14, 15, 12,
			// Top
			16, 17, 18, 18, 19, 16,
			// Bottom
			20, 21, 22, 22, 23, 20
		};

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Mesh::CreatePlane(float size)
	{
		float half = size / 2.0f;
		glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec3 normUp(0.0f, 1.0f, 0.0f);

		std::vector<Vertex> vertices = {
			Vertex(glm::vec4(-half, 0.0f, -half, 1.0f), normUp, white, glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec4( half, 0.0f, -half, 1.0f), normUp, white, glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec4( half, 0.0f,  half, 1.0f), normUp, white, glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec4(-half, 0.0f,  half, 1.0f), normUp, white, glm::vec2(0.0f, 1.0f)),
		};

		std::vector<unsigned int> indices = {
			0, 1, 2,
			2, 3, 0
		};

		return std::make_unique<Mesh>(vertices, indices);
	}
}

