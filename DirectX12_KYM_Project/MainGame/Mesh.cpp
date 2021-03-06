#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	if (m_VertexBuffer != nullptr) m_VertexBuffer->Release();
	if (m_UploadVertexBuffer != nullptr) m_UploadVertexBuffer->Release();

	if (m_IndexBuffer != nullptr) m_IndexBuffer->Release();
	if (m_UploadIndexBuffer != nullptr) m_UploadIndexBuffer->Release();
}

void Mesh::CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size)
{
	Vertex MeshVertex[36];

	m_VertexCount = 36;
	unsigned int ByteSize = sizeof(Vertex) * m_VertexCount;

	// 사각형 앞면의 위쪽 삼각형
	MeshVertex[0] = Vertex(DirectX::XMFLOAT3(-Size, +Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[1] = Vertex(DirectX::XMFLOAT3(+Size, +Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[2] = Vertex(DirectX::XMFLOAT3(+Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	// 사각형 앞면의 아래쪽 삼각형
	MeshVertex[3] = Vertex(DirectX::XMFLOAT3(-Size, +Size, -Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[4] = Vertex(DirectX::XMFLOAT3(+Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[5] = Vertex(DirectX::XMFLOAT3(-Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));

	// 사각형 윗면의 위쪽 삼각형
	MeshVertex[6] = Vertex(DirectX::XMFLOAT3(-Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	MeshVertex[7] = Vertex(DirectX::XMFLOAT3(+Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	MeshVertex[8] = Vertex(DirectX::XMFLOAT3(+Size, +Size, -Size), DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	// 사각형 윗면의 아래쪽 삼각형
	MeshVertex[9] = Vertex(DirectX::XMFLOAT3(-Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[10] = Vertex(DirectX::XMFLOAT3(+Size, +Size, -Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[11] = Vertex(DirectX::XMFLOAT3(-Size, +Size, -Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));

	// 사각형 뒷면의 위쪽 삼각형
	MeshVertex[12] = Vertex(DirectX::XMFLOAT3(-Size, -Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[13] = Vertex(DirectX::XMFLOAT3(+Size, -Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[14] = Vertex(DirectX::XMFLOAT3(+Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	// 사각형 뒷면의 아래쪽 삼각형
	MeshVertex[15] = Vertex(DirectX::XMFLOAT3(-Size, -Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[16] = Vertex(DirectX::XMFLOAT3(+Size, +Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[17] = Vertex(DirectX::XMFLOAT3(-Size, +Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));

	// 사각형 아랫면의 위쪽 삼각형
	MeshVertex[18] = Vertex(DirectX::XMFLOAT3(-Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[19] = Vertex(DirectX::XMFLOAT3(+Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	MeshVertex[20] = Vertex(DirectX::XMFLOAT3(+Size, -Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	// 사각형 아랫면의 아래쪽 삼각형
	MeshVertex[21] = Vertex(DirectX::XMFLOAT3(-Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[22] = Vertex(DirectX::XMFLOAT3(+Size, -Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[23] = Vertex(DirectX::XMFLOAT3(-Size, -Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));

	// 사각형 왼쪽면의 위쪽 삼각형
	MeshVertex[24] = Vertex(DirectX::XMFLOAT3(-Size, +Size, +Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[25] = Vertex(DirectX::XMFLOAT3(-Size, +Size, -Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	MeshVertex[26] = Vertex(DirectX::XMFLOAT3(-Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	// 사각형 왼쪽면의 아래쪽 삼각형
	MeshVertex[27] = Vertex(DirectX::XMFLOAT3(-Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[28] = Vertex(DirectX::XMFLOAT3(-Size, -Size, -Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));
	MeshVertex[29] = Vertex(DirectX::XMFLOAT3(-Size, -Size, +Size), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f));

	// 사각형 오른쪽면의 위쪽 삼각형
	MeshVertex[30] = Vertex(DirectX::XMFLOAT3(+Size, +Size, -Size), DirectX::XMFLOAT4(0.f, 0.5f, 0.f, 1.f));
	MeshVertex[31] = Vertex(DirectX::XMFLOAT3(+Size, +Size, +Size), DirectX::XMFLOAT4(0.f, 0.5f, 0.f, 1.f));
	MeshVertex[32] = Vertex(DirectX::XMFLOAT3(+Size, -Size, +Size), DirectX::XMFLOAT4(0.f, 0.5f, 0.f, 1.f));
	// 사각형 오른쪽면의 아래쪽 삼각형
	MeshVertex[33] = Vertex(DirectX::XMFLOAT3(+Size, +Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[34] = Vertex(DirectX::XMFLOAT3(+Size, -Size, +Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));
	MeshVertex[35] = Vertex(DirectX::XMFLOAT3(+Size, -Size, -Size), DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f));

	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = ByteSize;
}

ID3D12Resource* Mesh::CreateBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* MeshVertex, unsigned int Size, D3D12_RESOURCE_STATES ResourceState, ID3D12Resource *UploadBuffer)
{
	ID3D12Resource *Buffer = nullptr;

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
	ResourceDesc.Width = Size;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;

	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)&Buffer);

	// 업로드
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&UploadBuffer);

	D3D12_RANGE Range = { 0 };
	void* BufferDataBegin = nullptr;
	UploadBuffer->Map(0, &Range, (void**)&BufferDataBegin);
	memcpy(BufferDataBegin, MeshVertex, Size);
	UploadBuffer->Unmap(0, nullptr);

	CommandList->CopyResource(Buffer, UploadBuffer);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 리소스 사용 변화를 나타내는 전이 장벽
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = Buffer;
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter = ResourceState;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	return Buffer;
}

void Mesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

	CommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
}

// --------------------
TextureMesh::TextureMesh()
{

}

TextureMesh::~TextureMesh()
{

}

void TextureMesh::CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind)
{
	TextureVertex MeshVertex[6];

	m_VertexCount = 6;
	unsigned int ByteSize = sizeof(TextureVertex) * m_VertexCount;

	switch (Kind) {
	case 0:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;

	case 1:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;

	case 2:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;

	case 3:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;

	case 4:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;

	case 5:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.0f, 1.0f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.0f, 1.0f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.0f, 0.0f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.0f, 0.0f));
	}
	break;
	}
	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(TextureVertex);
	m_VertexBufferView.SizeInBytes = ByteSize;
}

// --------------------
TerrainMesh::TerrainMesh()
{

}

TerrainMesh::~TerrainMesh()
{

}

void TerrainMesh::CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Scale, int Width, int Length, BYTE* YPos)
{
	TerrainVertex *MeshVertex = new TerrainVertex[Width * Length];

	m_VertexCount = Width * Length;
	unsigned int ByteSize = sizeof(TerrainVertex) * m_VertexCount;

	for (int i = 0, z = 0; z < Length; ++z) {
		for (int x = 0; x < Width; ++x, ++i) {
			DirectX::XMFLOAT3 Position = { x * Scale.x, static_cast<float>(YPos[i]) * Scale.y, z * Scale.z };
			DirectX::XMFLOAT2 BaseUv = { static_cast<float>(x) / static_cast<float>(Width - 1), static_cast<float>(Length - 1 - z) / static_cast<float>(Length - 1) };
			DirectX::XMFLOAT2 DetailUv = { static_cast<float>(x) / static_cast<float>(Scale.x * 0.5f), static_cast<float>(z) / static_cast<float>(Scale.z * 0.5f) };
			MeshVertex[i] = TerrainVertex(Position, BaseUv, DetailUv, DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
			m_HeightMapYPos[x][z] = Position.y;
		}
	}
	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(TerrainVertex);
	m_VertexBufferView.SizeInBytes = ByteSize;

	delete[] MeshVertex;

	m_IndexCount = ((Width * 2) * (Length - 1)) + ((Length - 1) - 1);
	UINT *MeshIndex = new UINT[m_IndexCount];

	for (int i = 0, z = 0; z < Length - 1; ++z) {
		if (z % 2 == 0) {
			for (int x = 0; x < Width; ++x) {
				if (x == 0 && z > 0) MeshIndex[i++] = (UINT)(x + (z * Width));
				MeshIndex[i++] = (UINT)(x + (z * Width));
				MeshIndex[i++] = (UINT)((x + (z * Width)) + Width);
			}
		}
		else {
			for (int x = Width - 1; x >= 0; --x) {
				if (x == Width - 1) MeshIndex[i++] = (UINT)(x + (z * Width));
				MeshIndex[i++] = (UINT)(x + (z * Width));
				MeshIndex[i++] = (UINT)((x + (z * Width)) + Width);
			}
		}
	}
	m_IndexBuffer = CreateBuffer(Device, CommandList, MeshIndex, sizeof(UINT) * m_IndexCount, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_UploadIndexBuffer);

	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_IndexBufferView.SizeInBytes = sizeof(UINT) * m_IndexCount;

	delete[] MeshIndex;
}

void TerrainMesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

	CommandList->IASetIndexBuffer(&m_IndexBufferView);
	CommandList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}