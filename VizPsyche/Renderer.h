#pragma once

#include"Shader.h"
#include"VertexArray.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"

class Renderer
{
private:
public:
	void Clear(float clearColour[4]);
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

};