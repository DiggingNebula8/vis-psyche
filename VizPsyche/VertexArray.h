#ifndef VERTEX_ARRAY_CLASS_H
#define VERTEX_ARRAY_CLASS_H

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
	void Bind();
	// Unbinds the VertexArray
	void Unbind();
	// Deletes the VertexArray
	void Delete() const;
};
#endif