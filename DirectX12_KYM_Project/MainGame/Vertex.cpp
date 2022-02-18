#include "stdafx.h"
#include "Vertex.h"


Vertex::Vertex()
{

}

Vertex::Vertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Diffuse)
{
	m_Position = Position;
	m_Color = Diffuse;
}

Vertex::~Vertex()
{

}

// --------------------
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

// --------------------
BillboardVertex::BillboardVertex()
{

}

BillboardVertex::BillboardVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 Uv, int Kind)
{
	m_Position = Position;
	m_Uv = Uv;
	m_Kind = Kind;
}

BillboardVertex::~BillboardVertex()
{

}

// --------------------
TerrainVertex::TerrainVertex()
{

}

TerrainVertex::TerrainVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 BaseUv, DirectX::XMFLOAT2 DetailUv, DirectX::XMFLOAT4 Diffuse)
{
	m_Position = Position;
	m_BaseUv = BaseUv;
	m_DetailUv = DetailUv;
	m_Diffuse = Diffuse;
}

TerrainVertex::~TerrainVertex()
{

}