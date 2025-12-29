#include"IndexBuffer.h"

// Constructor that generates a Elements Buffer Object and links it to indices
IndexBuffer::IndexBuffer(const unsigned int* indices, unsigned int count)
	: m_ID(0), m_Count(count)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
}

// Move constructor
IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
	: m_ID(other.m_ID), m_Count(other.m_Count)
{
	other.m_ID = 0;
	other.m_Count = 0;
}

// Move assignment operator
IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
	if (this != &other)
	{
		if (m_ID != 0)
		{
			glDeleteBuffers(1, &m_ID);
		}
		m_ID = other.m_ID;
		m_Count = other.m_Count;
		other.m_ID = 0;
		other.m_Count = 0;
	}
	return *this;
}

// Binds the IndexBuffer
void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

// Unbinds the IndexBuffer
void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}