#pragma once
// ������Ʈ�� ����ϴ� ����
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

// �ؽ��� ������ �ϴ� ������Ʈ�� ����ϴ� ����
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

// ������ ���Ǵ� ����
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