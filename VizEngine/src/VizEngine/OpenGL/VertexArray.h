#pragma once

#include <glad/glad.h>
#include "VizEngine/Core.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace VizEngine
{
	class VizEngine_API VertexArray
	{
	public:
		// Constructor that generates a VertexArray ID
		VertexArray();
		~VertexArray();

		// Prevent copying (Rule of 5)
		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;

		// Allow moving
		VertexArray(VertexArray&& other) noexcept;
		VertexArray& operator=(VertexArray&& other) noexcept;

		// Links a VertexBuffer to the VertexArray using a certain layout
		void LinkVertexBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout) const;
		// Binds the VertexArray
		void Bind() const;
		// Unbinds the VertexArray
		void Unbind() const;

		// Getter for ID
		inline unsigned int GetID() const { return m_ID; }

	private:
		unsigned int m_ID;
	};
}