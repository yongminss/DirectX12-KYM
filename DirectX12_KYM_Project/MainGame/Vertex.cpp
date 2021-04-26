#include "stdafx.h"
#include "Vertex.h"


Vertex::Vertex()
{

}

Vertex::Vertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Diffuse)
{
	m_Position = Position;
	m_Diffuse = Diffuse;
}

Vertex::~Vertex()
{

}


TextureVertex::TextureVertex()
{

}

TextureVertex::TextureVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 Uv)
{
	m_Position = Position;
	m_Uv = Uv;
}

TextureVertex::~TextureVertex()
{

}