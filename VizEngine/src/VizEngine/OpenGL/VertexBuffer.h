#pragma once

#include<glad/glad.h>

class VertexBuffer
{
public:
	// Reference ID of the Vertex Buffer Object
	unsigned int ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VertexBuffer(const void* vertices, unsigned int size);
	~VertexBuffer();

	// Binds the VertexBuffer
	void Bind() const;
	// Unbinds the VertexBuffer
	void Unbind() const;
	// Deletes the VertexBuffer
	void Delete() const;
};