#pragma once
// 오브젝트가 사용하는 정점
class Vertex
{
private:
	DirectX::XMFLOAT3 m_Position = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT4 m_Color = { 0.f ,0.f, 0.f, 1.f };

public:
	Vertex();
	Vertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Diffuse);
	~Vertex();
};

// 텍스쳐 맵핑을 하는 오브젝트가 사용하는 정점
class TextureVertex
{
protected:
	DirectX::XMFLOAT3 m_Position = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT2 m_Uv = { 0.f, 0.f };
	int m_Kind = 0;

public:
	TextureVertex();
	TextureVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 Uv, int Kind);
	~TextureVertex();
};

// 지형에 사용되는 정점
class TerrainVertex
{
private:
	DirectX::XMFLOAT3 m_Position = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT2 m_BaseUv = { 0.f, 0.f };
	DirectX::XMFLOAT2 m_DetailUv = { 0.f, 0.f };

public:
	TerrainVertex();
	TerrainVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 BaseUv, DirectX::XMFLOAT2 DetailUv);
	~TerrainVertex();
};