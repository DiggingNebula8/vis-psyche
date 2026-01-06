#include "VertexBuffer.h"

namespace VizEngine
{
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VertexBuffer::VertexBuffer(const void* vertices, unsigned int size)
		: m_vbo(0)
	{
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (m_vbo != 0)
		{
			glDeleteBuffers(1, &m_vbo);
		}
	}

	// Move constructor
	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
		: m_vbo(other.m_vbo)
	{
		other.m_vbo = 0;
	}

	// Move assignment operator
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			if (m_vbo != 0)
			{
				glDeleteBuffers(1, &m_vbo);
			}
			m_vbo = other.m_vbo;
			other.m_vbo = 0;
		}
		return *this;
	}

	// Binds the VertexBuffer
	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	}

	// Unbinds the VertexBuffer
	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
