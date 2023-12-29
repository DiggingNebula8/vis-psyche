#pragma once

#include<glad/glad.h>
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"

class VertexArray
{
public:
	// ID reference for the Vertex Array Object
	unsigned int ID;
	// Constructor that generates a VertexArray ID
	VertexArray();
	~VertexArray();

	// Links a VertexBuffer to the VertexArray using a certain layout
	void LinkVertexBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout);
	// Binds the VertexArray
	void Bind() const;
	// Unbinds the VertexArray
	void Unbind() const;
	// Deletes the VertexArray
	void Delete() const;
};