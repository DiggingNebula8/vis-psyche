// VizEngine/src/VizEngine/OpenGL/FullscreenQuad.h

#pragma once

#include "VizEngine/Core.h"
#include <memory>

namespace VizEngine
{
	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	/**
	 * Simple fullscreen quad for post-processing effects.
	 * Covers the entire screen in NDC coordinates [-1, 1].
	 */
	class VizEngine_API FullscreenQuad
	{
	public:
		FullscreenQuad();
		~FullscreenQuad() = default;

		/**
		 * Render the fullscreen quad.
		 * Call this after binding your post-processing shader.
		 */
		void Render();

	private:
		std::shared_ptr<VertexArray> m_VAO;
		std::shared_ptr<VertexBuffer> m_VBO;
		std::shared_ptr<IndexBuffer> m_IBO;
	};
}
