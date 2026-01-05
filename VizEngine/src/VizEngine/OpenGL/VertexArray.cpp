#include "VertexArray.h"

namespace VizEngine
{
	// Constructor that generates a VertexArray ID
	VertexArray::VertexArray()
		: m_vao(0)
	{
		glGenVertexArrays(1, &m_vao);
	}

	VertexArray::~VertexArray()
	{
		if (m_vao != 0)
		{
			glDeleteVertexArrays(1, &m_vao);
		}
	}

	// Move constructor
	VertexArray::VertexArray(VertexArray&& other) noexcept
		: m_vao(other.m_vao)
	{
		other.m_vao = 0;
	}

	// Move assignment operator
	VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
	{
		if (this != &other)
		{
			if (m_vao != 0)
			{
				glDeleteVertexArrays(1, &m_vao);
			}
			m_vao = other.m_vao;
			other.m_vao = 0;
		}
		return *this;
	}

	// Links a VertexBuffer to the VertexArray using a certain layout
	void VertexArray::LinkVertexBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout) const
	{
		Bind();
		vertexBuffer.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), reinterpret_cast<const void*>(static_cast<size_t>(offset)));
			offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		}
	}

	// Binds the VertexArray
	void VertexArray::Bind() const
	{
		glBindVertexArray(m_vao);
	}

	// Unbinds the VertexArray
	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}