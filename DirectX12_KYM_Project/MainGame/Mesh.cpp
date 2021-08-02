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

// --------------------
LoadFileMesh::LoadFileMesh()
{

}

LoadFileMesh::~LoadFileMesh()
{

}

void LoadFileMesh::LoadFile(FILE* File)
{
	/*char Word[64] = { '\0' };

	fread(&m_VertexCount, sizeof(int), 1, File);

	std::cout << "Vertex Count : " << m_VertexCount << std::endl;

	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(Word, sizeof(char), WordCount, File);

	Word[WordCount] = '\0';

	std::cout << "Word Count : " << int(WordCount) << std::endl;
	std::cout << "Word : " << Word << std::endl;

	while (true) {

		fread(&WordCount, sizeof(BYTE), 1, File);
		fread(Word, sizeof(char), WordCount, File);

		Word[WordCount] = '\0';

		std::cout << "Word Count : " << int(WordCount) << std::endl;
		std::cout << "Word : " << Word << std::endl;

		if (!strcmp(Word, "<Bounds>:")) {

			DirectX::XMFLOAT3 AabbCenter{}, AAbbExtent{};

			fread(&AabbCenter, sizeof(float), 3, File);
			fread(&AAbbExtent, sizeof(float), 3, File);

			std::cout << "AABB Center : " << AabbCenter.x << ", " << AabbCenter.y << ", " << AabbCenter.z << std::endl;
			std::cout << "AABB Extent : " << AAbbExtent.x << ", " << AAbbExtent.y << ", " << AAbbExtent.z << std::endl;
		}

		else if (!strcmp(Word, "<Positions>:")) {

			int PositionCount = 0;
			fread(&PositionCount, sizeof(int), 1, File);

			std::cout << "Position Count : " << PositionCount << std::endl;

			if (PositionCount > 0) {
				DirectX::XMFLOAT3 *Position = new DirectX::XMFLOAT3[PositionCount];

				fread(Position, sizeof(DirectX::XMFLOAT3), PositionCount, File);

				for (int i = 0; i < PositionCount; ++i) {
					std::cout << '[' << i << "] Position : " << Position[i].x << ", " << Position[i].y << ", " << Position[i].z << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<Colors>:")) {

			int ColorCount = 0;
			fread(&ColorCount, sizeof(int), 1, File);

			std::cout << "Color Count : " << ColorCount << std::endl;
		}

		else if (!strcmp(Word, "<TextureCoords0>:")) {
			int UvCount = 0;
			fread(&UvCount, sizeof(int), 1, File);

			std::cout << "Uv 0 Count : " << UvCount << std::endl;

			if (UvCount > 0) {
				DirectX::XMFLOAT2 *Uv = new DirectX::XMFLOAT2[UvCount];

				fread(Uv, sizeof(DirectX::XMFLOAT2), UvCount, File);

				for (int i = 0; i < UvCount; ++i) {
					std::cout << '[' << i << "] Uv 0 : " << Uv[i].x << ", " << Uv[i].y << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<TextureCoords1>:")) {
			int UvCount = 0;
			fread(&UvCount, sizeof(int), 1, File);

			std::cout << "Uv 1 Count : " << UvCount << std::endl;

			if (UvCount > 0) {
				DirectX::XMFLOAT2 *Uv = new DirectX::XMFLOAT2[UvCount];

				fread(Uv, sizeof(DirectX::XMFLOAT2), UvCount, File);

				for (int i = 0; i < UvCount; ++i) {
					std::cout << '[' << i << "] Uv 1 : " << Uv[i].x << ", " << Uv[i].y << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<Normals>:")) {
			int NormalCount = 0;
			fread(&NormalCount, sizeof(int), 1, File);

			std::cout << "Normal Count : " << NormalCount << std::endl;

			if (NormalCount > 0) {
				DirectX::XMFLOAT3 *Normal = new DirectX::XMFLOAT3[NormalCount];

				fread(Normal, sizeof(DirectX::XMFLOAT3), NormalCount, File);

				for (int i = 0; i < NormalCount; ++i) {
					std::cout << '[' << i << "] Normal : " << Normal[i].x << ", " << Normal[i].y << ", " << Normal[i].z << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<Tangents>:")) {
			int TangentCount = 0;
			fread(&TangentCount, sizeof(int), 1, File);

			std::cout << "Tangent Count : " << TangentCount << std::endl;

			if (TangentCount > 0) {
				DirectX::XMFLOAT3 *Tangent = new DirectX::XMFLOAT3[TangentCount];

				fread(Tangent, sizeof(DirectX::XMFLOAT3), TangentCount, File);

				for (int i = 0; i < TangentCount; ++i) {
					std::cout << '[' << i << "] Tangent : " << Tangent[i].x << ", " << Tangent[i].y << ", " << Tangent[i].z << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<BiTangents>:")) {

			int BiTangentCount = 0;
			fread(&BiTangentCount, sizeof(int), 1, File);

			std::cout << "BiTangent Count : " << BiTangentCount << std::endl;

			if (BiTangentCount > 0) {
				DirectX::XMFLOAT3 *BiTangent = new DirectX::XMFLOAT3[BiTangentCount];

				fread(BiTangent, sizeof(DirectX::XMFLOAT3), BiTangentCount, File);

				for (int i = 0; i < BiTangentCount; ++i) {
					std::cout << '[' << i << "] BiTangent : " << BiTangent[i].x << ", " << BiTangent[i].y << ", " << BiTangent[i].z << std::endl;
				}
			}
		}

		else if (!strcmp(Word, "<SubMeshes>:")) {

			int SubMeshCount = 0;
			fread(&SubMeshCount, sizeof(int), 1, File);

			std::cout << "SubMesh Count : " << SubMeshCount << std::endl;

			if (SubMeshCount > 0) {
				for (int i = 0; i < SubMeshCount; ++i) {

					fread(&WordCount, sizeof(BYTE), 1, File);
					fread(Word, sizeof(char), WordCount, File);

					Word[WordCount] = '\0';

					if (!strcmp(Word, "<SubMesh>:")) {

						int IndexCount = 0;

						fread(&IndexCount, sizeof(int), 1, File);
						fread(&m_IndexCount, sizeof(int), 1, File);

						std::cout << "Index Count : " << IndexCount << std::endl;
						std::cout << "Index Count of Member : " << m_IndexCount << std::endl;

						if (m_IndexCount > 0) {
							UINT **SubIndex = new UINT*[SubMeshCount];
							SubIndex[i] = new UINT[m_IndexCount];
							fread(&SubIndex[i], sizeof(UINT), m_IndexCount, File);
							std::cout << "Sub Index Buffer : " << SubIndex[i] << std::endl;
						}
					}
				}
			}
		}

		else if (!strcmp(Word, "</Mesh>")) {
			break;
		}
	}*/


	char Token[64] = { '\0' };
	BYTE nStrLength = 0;

	int nPosition = 0, nColor = 0, nNormal = 0, nTangent = 0, nBiTangent = 0, nTextureCoord = 0, nIndice = 0, nSubMesh = 0, nSubIndice = 0;

	fread(&m_VertexCount, sizeof(int), 1, File);
	fread(&nStrLength, sizeof(BYTE), 1, File);
	fread(&m_MeshName, sizeof(char), nStrLength, File);
	m_MeshName[nStrLength] = '\0';

	for (; ;) {
		fread(&nStrLength, sizeof(BYTE), 1, File);
		fread(&Token, sizeof(char), nStrLength, File);
		Token[nStrLength] = '\0';

		std::cout << "Word In Mesh : " << Token << std::endl;

		if (!strcmp(Token, "<Bounds>:")) {
			fread(&m_AabbCenter, sizeof(DirectX::XMFLOAT3), 1, File);
			fread(&m_AAbbExtent, sizeof(DirectX::XMFLOAT3), 1, File);
		}

		else if (!strcmp(Token, "<Positions>:")) {
			fread(&nPosition, sizeof(int), 1, File);
			if (nPosition > 0) {
				/*m_nType |= VERTEXT_POSITION;
				m_Position = new XMFLOAT3[nPosition];*/
				fread(&m_Position, sizeof(DirectX::XMFLOAT3), nPosition, File);

				/*m_PositionBuffer = ::CreateBufferResource(Device, CommandList, m_Position, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_PositionUploadBuffer);

				m_PositionBufferView.BufferLocation = m_PositionBuffer->GetGPUVirtualAddress();
				m_PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;*/
			}
		}

		else if (!strcmp(Token, "<Colors>:")) {
			fread(&nColor, sizeof(int), 1, File);
			if (nColor > 0) {
				/*m_nType |= VERTEXT_COLOR;
				m_Color = new XMFLOAT4[nColor];*/
				fread(&m_Color, sizeof(DirectX::XMFLOAT4), nColor, File);
			}
		}

		else if (!strcmp(Token, "<TextureCoords0>:")) {
			fread(&nTextureCoord, sizeof(int), 1, File);
			if (nTextureCoord > 0) {
				/*m_nType |= VERTEXT_TEXTURE_COORD0;
				m_TextureCoord0 = new XMFLOAT2[nTextureCoord];*/
				fread(&m_Uv0, sizeof(DirectX::XMFLOAT2), nTextureCoord, File);

				/*m_TextureCoord0Buffer = ::CreateBufferResource(Device, CommandList, m_TextureCoord0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TextureCoord0UploadBuffer);

				m_TextureCoord0BufferView.BufferLocation = m_TextureCoord0Buffer->GetGPUVirtualAddress();
				m_TextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_TextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;*/
			}
		}
		else if (!strcmp(Token, "<TextureCoords1>:")) {
			fread(&nTextureCoord, sizeof(int), 1, File);
			if (nTextureCoord > 0) {
				/*m_nType |= VERTEXT_TEXTURE_COORD1;
				m_TextureCoord1 = new XMFLOAT2[nTextureCoord];*/
				fread(&m_Uv1, sizeof(DirectX::XMFLOAT2), nTextureCoord, File);

				/*m_TextureCoord1Buffer = ::CreateBufferResource(Device, CommandList, m_TextureCoord1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TextureCoord1UploadBuffer);

				m_TextureCoord1BufferView.BufferLocation = m_TextureCoord1Buffer->GetGPUVirtualAddress();
				m_TextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_TextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;*/
			}
		}

		else if (!strcmp(Token, "<Normals>:")) {
			fread(&nNormal, sizeof(int), 1, File);
			if (nNormal > 0) {
				/*m_nType |= VERTEXT_NORMAL;
				m_Normal = new XMFLOAT3[nNormal];*/
				fread(&m_Normal, sizeof(DirectX::XMFLOAT3), nNormal, File);

				/*m_NormalBuffer = ::CreateBufferResource(Device, CommandList, m_Normal, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_NormalUploadBuffer);

				m_NormalBufferView.BufferLocation = m_NormalBuffer->GetGPUVirtualAddress();
				m_NormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_NormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;*/
			}
		}

		else if (!strcmp(Token, "<Tangents>:")) {
			fread(&nTangent, sizeof(int), 1, File);
			if (nTangent > 0) {
				/*m_nType |= VERTEXT_TANGENT;
				m_Tangent = new XMFLOAT3[nTangent];*/
				fread(&m_Tangent, sizeof(DirectX::XMFLOAT3), nTangent, File);

				/*m_TangentBuffer = ::CreateBufferResource(Device, CommandList, m_Tangent, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TangentUploadBuffer);

				m_TangentBufferView.BufferLocation = m_TangentBuffer->GetGPUVirtualAddress();
				m_TangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_TangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;*/
			}
		}

		else if (!strcmp(Token, "<BiTangents>:")) {
			fread(&nBiTangent, sizeof(int), 1, File);
			if (nBiTangent > 0) {
				/*m_BiTangent = new XMFLOAT3[nBiTangent];*/
				fread(&m_BiTangent, sizeof(DirectX::XMFLOAT3), nBiTangent, File);

				/*m_BiTangentBuffer = ::CreateBufferResource(Device, CommandList, m_BiTangent, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BiTangentUploadBuffer);

				m_BiTangentBufferView.BufferLocation = m_BiTangentBuffer->GetGPUVirtualAddress();
				m_BiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_BiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;*/
			}
		}

		else if (!strcmp(Token, "<SubMeshes>:")) {
		fread(&m_SubMeshCount, sizeof(int), 1, File);
			if (m_SubMeshCount > 0) {
				/*m_pnSubSetIndices = new int[m_nSubMesh];
				m_ppnSubSetIndices = new UINT*[m_nSubMesh];

				m_SubSetIndexBuffer = new ID3D12Resource*[m_nSubMesh];
				m_SubSetIndexUploadBuffer = new ID3D12Resource*[m_nSubMesh];
				m_SubSetIndexBufferView = new D3D12_INDEX_BUFFER_VIEW[m_nSubMesh];*/

				for (int i = 0; i < m_SubMeshCount; ++i) {
					fread(&nStrLength, sizeof(BYTE), 1, File);
					fread(Token, sizeof(char), nStrLength, File);
					Token[nStrLength] = '\0';
					if (!strcmp(Token, "<SubMesh>:")) {
						int nIndex = 0;
						fread(&nIndex, sizeof(int), 1, File);
						fread(&m_IndexCount, sizeof(int), 1, File);

						if (m_IndexCount > 0) {
							/*m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];*/
							fread(&m_tempSubMeshCount, sizeof(UINT), m_IndexCount, File);

							/*m_SubSetIndexBuffer[i] = ::CreateBufferResource(Device, CommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_SubSetIndexUploadBuffer[i]);

							m_SubSetIndexBufferView[i].BufferLocation = m_SubSetIndexBuffer[i]->GetGPUVirtualAddress();
							m_SubSetIndexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
							m_SubSetIndexBufferView[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];*/
						}
					}
				}
			}
		}
		else if (!strcmp(Token, "</Mesh>")) {
			break;
		}
	}
}