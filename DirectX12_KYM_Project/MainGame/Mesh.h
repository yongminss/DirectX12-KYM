#pragma once

#include "Vertex.h"

// 오브젝트가 사용할 Vertex Buffer 생성 및 관리
class Mesh
{
protected:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	ID3D12Resource *m_IndexBuffer = nullptr;
	ID3D12Resource *m_UploadIndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView{};

	unsigned int m_VertexCount = 0;
	unsigned int m_IndexCount = 0;

	float m_HeightMapYPos[257][257]{};

public:
	Mesh();
	~Mesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size);
	ID3D12Resource *CreateBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* MeshVertex, unsigned int Size, D3D12_RESOURCE_STATES ResourceState, ID3D12Resource *UploadBuffer);

	float GetHeightMapYPos(int x, int z) { return m_HeightMapYPos[x][z]; }

	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};

// 텍스처 매핑을 하는 오브젝트가 사용하는 Mesh
class TextureMesh : public Mesh
{
public:
	TextureMesh();
	~TextureMesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind);
};

// 지형에 사용될 Mesh
class TerrainMesh : public Mesh
{
public:
	TerrainMesh();
	~TerrainMesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Scale, int Width, int Length, BYTE* YPos);

	void Render(ID3D12GraphicsCommandList* CommandList);
};