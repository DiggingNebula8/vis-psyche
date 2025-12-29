#pragma once

#include<glad/glad.h>

class VertexBuffer
{
public:
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VertexBuffer(const void* vertices, unsigned int size);
	~VertexBuffer();

	// Prevent copying (Rule of 5)
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	// Allow moving
	VertexBuffer(VertexBuffer&& other) noexcept;
	VertexBuffer& operator=(VertexBuffer&& other) noexcept;

	// Binds the VertexBuffer
	void Bind() const;
	// Unbinds the VertexBuffer
	void Unbind() const;

	// Getter for ID
	inline unsigned int GetID() const { return m_ID; }

private:
	unsigned int m_ID;
};