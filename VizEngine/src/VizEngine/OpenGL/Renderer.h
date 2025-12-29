#pragma once

#include"Shader.h"
#include"VertexArray.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"

class Renderer
{
public:
	void Clear(float clearColor[4]);
	void ClearDepth();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};