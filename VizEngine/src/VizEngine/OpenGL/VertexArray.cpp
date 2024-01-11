#include "vepch.h"

#include"VertexArray.h"

// Constructor that generates a VertexArray ID
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &ID);
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
	//vertexBuffer.Unbind();
}

// Binds the VertexArray
void VertexArray::Bind() const
{
	glBindVertexArray(ID);
}

// Unbinds the VertexArray
void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

// Deletes the VertexArray
void VertexArray::Delete() const
{
	glDeleteVertexArrays(1, &ID);
}