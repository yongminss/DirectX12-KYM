#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	if (m_VertexBuffer != nullptr) m_VertexBuffer->Release();
	if (m_UploadVertexBuffer != nullptr) m_UploadVertexBuffer->Release();
}

void Mesh::CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	Vertex MeshVertex[36];

	m_VertexCount = 36;
	unsigned int ByteSize = sizeof(Vertex) * m_VertexCount;

	// 사각형 앞면의 위쪽 삼각형
	MeshVertex[0] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[1] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[2] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	// 사각형 앞면의 아래쪽 삼각형
	MeshVertex[3] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, -10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[4] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[5] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));

	// 사각형 윗면의 위쪽 삼각형
	MeshVertex[6] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	MeshVertex[7] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	MeshVertex[8] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, -10.f), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	// 사각형 윗면의 아래쪽 삼각형
	MeshVertex[9] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[10] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, -10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[11] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, -10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));

	// 사각형 뒷면의 위쪽 삼각형
	MeshVertex[12] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[13] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[14] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	// 사각형 뒷면의 아래쪽 삼각형
	MeshVertex[15] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[16] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[17] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));

	// 사각형 아랫면의 위쪽 삼각형
	MeshVertex[18] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[19] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[20] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	// 사각형 아랫면의 아래쪽 삼각형
	MeshVertex[21] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[22] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[23] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));

	// 사각형 왼쪽면의 위쪽 삼각형
	MeshVertex[24] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, +10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[25] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, -10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[26] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	// 사각형 왼쪽면의 아래쪽 삼각형
	MeshVertex[27] = Vertex(DirectX::XMFLOAT3(-10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[28] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, -10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[29] = Vertex(DirectX::XMFLOAT3(-10.f, -10.f, +10.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));

	// 사각형 오른쪽면의 위쪽 삼각형
	MeshVertex[30] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, -10.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	MeshVertex[31] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, +10.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	MeshVertex[32] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, +10.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	// 사각형 오른쪽면의 아래쪽 삼각형
	MeshVertex[33] = Vertex(DirectX::XMFLOAT3(+10.f, +10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[34] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, +10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[35] = Vertex(DirectX::XMFLOAT3(+10.f, -10.f, -10.f), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));

	CreateVertexBuffer(Device, CommandList, MeshVertex, ByteSize);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = ByteSize;
}

void Mesh::CreateVertexBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* MeshVertex, unsigned int size)
{
	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = size;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;

	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)&m_VertexBuffer);

	// 업로드
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&m_UploadVertexBuffer);

	D3D12_RANGE Range = { 0 };
	void* BufferDataBegin = nullptr;
	m_UploadVertexBuffer->Map(0, &Range, (void**)&BufferDataBegin);
	memcpy(BufferDataBegin, MeshVertex, size);
	m_UploadVertexBuffer->Unmap(0, nullptr);

	CommandList->CopyResource(m_VertexBuffer, m_UploadVertexBuffer);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 리소스 사용 변화를 나타내는 전이 장벽
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_VertexBuffer;
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);
}

void Mesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

	CommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
}