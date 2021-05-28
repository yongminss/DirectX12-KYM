#pragma once

#include "Vertex.h"

// 오브젝트가 사용할 Vertex Buffer 생성 및 관리
class Mesh
{
protected:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	unsigned int m_VertexCount = 0;

public:
	Mesh();
	~Mesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size);
	void CreateVertexBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* MeshVertex, unsigned int Size);

	void Render(ID3D12GraphicsCommandList* CommandList);
};

class TextureMesh : public Mesh
{
public:
	TextureMesh();
	~TextureMesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind);
};

class TerrainMesh : public Mesh
{
public:
	TerrainMesh();
	~TerrainMesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size, BYTE* VertexYPosition);

	void Render(ID3D12GraphicsCommandList* CommandList);
};