#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VizEngine/Core.h"

namespace VizEngine
{
	class VizEngine_API Renderer
	{
	public:
		void Clear(float clearColor[4]);
		void ClearDepth();
		void SetViewport(int x, int y, int width, int height);
		void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

		// Shadow mapping helpers
		void EnablePolygonOffset(float factor, float units);
		void DisablePolygonOffset();
	};
}