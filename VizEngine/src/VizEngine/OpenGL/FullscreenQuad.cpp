// VizEngine/src/VizEngine/OpenGL/FullscreenQuad.cpp

#include "FullscreenQuad.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

#include <glad/glad.h>

namespace VizEngine
{
	FullscreenQuad::FullscreenQuad()
	{
		// Fullscreen quad vertices (NDC coordinates)
		// Position (x, y, z) + TexCoords (u, v)
		float vertices[] = {
			// Positions        // TexCoords
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
		};

		// Indices for two triangles
		unsigned int indices[] = {
			0, 1, 2,  // First triangle
			2, 3, 0   // Second triangle
		};

		// Create vertex buffer
		m_VBO = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));

		// Define layout
		VertexBufferLayout layout;
		layout.Push<float>(3);  // Position
		layout.Push<float>(2);  // TexCoords

		// Create vertex array and link buffer
		m_VAO = std::make_shared<VertexArray>();
		m_VAO->LinkVertexBuffer(*m_VBO, layout);

		// Create index buffer
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
	}

	void FullscreenQuad::Render()
	{
		m_VAO->Bind();
		m_IBO->Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
}
