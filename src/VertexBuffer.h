#pragma once
#include<GLEW/glew.h>
class VertexBuffer
{
private:
	unsigned int m_RendererId;
public:
	VertexBuffer();
	~VertexBuffer();

	void Bind();
	void UnBind();
};