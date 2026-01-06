#include "IndexBuffer.h"

namespace VizEngine
{
	// Constructor that generates a Elements Buffer Object and links it to indices
	IndexBuffer::IndexBuffer(const unsigned int* indices, unsigned int count)
		: m_ibo(0), m_Count(count)
	{
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		if (m_ibo != 0)
		{
			glDeleteBuffers(1, &m_ibo);
		}
	}

	// Move constructor
	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
		: m_ibo(other.m_ibo), m_Count(other.m_Count)
	{
		other.m_ibo = 0;
		other.m_Count = 0;
	}

	// Move assignment operator
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ibo != 0)
			{
				glDeleteBuffers(1, &m_ibo);
			}
			m_ibo = other.m_ibo;
			m_Count = other.m_Count;
			other.m_ibo = 0;
			other.m_Count = 0;
		}
		return *this;
	}

	// Binds the IndexBuffer
	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	}

	// Unbinds the IndexBuffer
	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}