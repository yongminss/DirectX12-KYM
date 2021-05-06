#pragma once
// ������Ʈ�� ����ϴ� ����
class Vertex
{
private:
	DirectX::XMFLOAT3 m_Position = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT4 m_Diffuse = { 0.f ,0.f, 0.f, 1.f };

public:
	Vertex();
	Vertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Diffuse);
	~Vertex();
};

// �ؽ��� ������ �ϴ� ������Ʈ�� ����ϴ� ����
class TextureVertex
{
private:
	DirectX::XMFLOAT3 m_Position = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT2 m_Uv = { 0.f, 0.f };

public:
	TextureVertex();
	TextureVertex(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT2 Uv);
	~TextureVertex();
};