#pragma once

#include <glad/glad.h>
#include "VizEngine/Core.h"

namespace VizEngine
{
	class VizEngine_API IndexBuffer
	{
	public:
		// Constructor that generates a Elements Buffer Object and links it to indices
		IndexBuffer(const unsigned int* indices, unsigned int count);
		~IndexBuffer();

		// Prevent copying (Rule of 5)
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		// Allow moving
		IndexBuffer(IndexBuffer&& other) noexcept;
		IndexBuffer& operator=(IndexBuffer&& other) noexcept;

		// Binds the IndexBuffer
		void Bind() const;
		// Unbinds the IndexBuffer
		void Unbind() const;

		// Getters
		inline unsigned int GetID() const { return m_ID; }
		inline unsigned int GetCount() const { return m_Count; }

	private:
		unsigned int m_ID;
		unsigned int m_Count;
	};
}