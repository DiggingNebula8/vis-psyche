#pragma once

#include<glad/glad.h>

class IndexBuffer
{
public:
	// ID reference of Elements Buffer Object
	unsigned int ID;
	unsigned int count;
	// Constructor that generates a Elements Buffer Object and links it to indices
	IndexBuffer(const unsigned int* indices, unsigned int count);
	~IndexBuffer();
	// Binds the IndexBuffer
	void Bind() const;
	// Unbinds the IndexBuffer
	void Unbind() const;
	// Deletes the IndexBuffer
	void Delete() const;

	inline unsigned int GetCount() const { return count; }
};