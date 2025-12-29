#include"VertexBuffer.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VertexBuffer::VertexBuffer(const void* vertices, unsigned int size)
	: m_ID(0)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
}

// Move constructor
VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
	: m_ID(other.m_ID)
{
	other.m_ID = 0;
}

// Move assignment operator
VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
	if (this != &other)
	{
		if (m_ID != 0)
		{
			glDeleteBuffers(1, &m_ID);
		}
		m_ID = other.m_ID;
		other.m_ID = 0;
	}
	return *this;
}

// Binds the VertexBuffer
void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

// Unbinds the VertexBuffer
void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
