#pragma once

#include "Vertex.h"

// ������Ʈ�� ����� Vertex Buffer ���� �� ����
class Mesh
{
private:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	unsigned int m_VertexCount = 0;

public:
	Mesh();
	~Mesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateVertexBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* MeshVertex, unsigned int Size);

	void Render(ID3D12GraphicsCommandList* CommandList);
};