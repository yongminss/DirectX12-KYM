#include "stdafx.h"
#include "Mesh.h"

#include "Vertex.h"
#include "GameObject.h"


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

Mesh::Mesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size)
{
	Vertex MeshVertex[36];

	m_VertexCount = 36;
	int ByteSize = sizeof(Vertex) * m_VertexCount;

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

	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = ByteSize;
}

void Mesh::UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList)
{
	if (m_VertexBuffer != nullptr) {
		CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	UpdateVertexBufferView(CommandList);

	if (m_IndexBuffer != nullptr) {
		CommandList->IASetIndexBuffer(&m_IndexBufferView);
		CommandList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
	}
	else {
		CommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView, int ModelCount)
{
	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	UpdateVertexBufferView(CommandList, InstanceBufferView);

	if (m_IndexBuffer != nullptr) {
		CommandList->IASetIndexBuffer(&m_IndexBufferView);
		CommandList->DrawIndexedInstanced(m_IndexCount, ModelCount, 0, 0, 0);
	}
	else {
		CommandList->DrawInstanced(m_VertexCount, ModelCount, 0, 0);
	}
}

// --------------------
TextureMesh::TextureMesh()
{

}

TextureMesh::~TextureMesh()
{

}

TextureMesh::TextureMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind, int NumbersIndex)
{
	TextureVertex MeshVertex[6];

	m_VertexCount = 6;
	unsigned int ByteSize = sizeof(TextureVertex) * m_VertexCount;

	switch (Kind) {
	case 0:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 1:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 2:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 3:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(Size.x, -Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(Size.x, +Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 4:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 5:
	{
		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 0.f));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(1.f, 1.f));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 1.f));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, -Size.z), DirectX::XMFLOAT2(0.f, 1.f));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(0.f, 0.f));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, Size.y, +Size.z), DirectX::XMFLOAT2(1.f, 0.f));
	}
	break;

	case 6: // UV 값을 다르게 설정할 텍스처에 사용 (ex. Numbers)
	{
		const float UvX = 0.25f, UvY = 0.32f;

		if (NumbersIndex > 9) NumbersIndex -= 10;

		float AddX = 0.f, AddY = 0.f;

		AddX = UvX * (NumbersIndex % 4);

		if (NumbersIndex >= 4 && NumbersIndex < 8) AddY = UvY * 1.f;
		else if (NumbersIndex >= 8) AddY = UvY * 2.f;

		MeshVertex[0] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(UvX + AddX, 0.f + AddY));
		MeshVertex[1] = TextureVertex(DirectX::XMFLOAT3(+Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(UvX + AddX, UvY + AddY));
		MeshVertex[2] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f + AddX, UvY + AddY));

		MeshVertex[3] = TextureVertex(DirectX::XMFLOAT3(-Size.x, -Size.y, Size.z), DirectX::XMFLOAT2(0.f + AddX, UvY + AddY));
		MeshVertex[4] = TextureVertex(DirectX::XMFLOAT3(-Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(0.f + AddX, 0.f + AddY));
		MeshVertex[5] = TextureVertex(DirectX::XMFLOAT3(+Size.x, +Size.y, Size.z), DirectX::XMFLOAT2(UvX + AddX, 0.f + AddY));
	}
	break;

	}
	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

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

TerrainMesh::TerrainMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Scale, int Width, int Length, BYTE* YPos)
{
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

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
	m_VertexBuffer = CreateBuffer(Device, CommandList, MeshVertex, ByteSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

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
	m_IndexBuffer = CreateBuffer(Device, CommandList, MeshIndex, sizeof(UINT) * m_IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_UploadIndexBuffer);

	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_IndexBufferView.SizeInBytes = sizeof(UINT) * m_IndexCount;

	delete[] MeshIndex;
}

// --------------------
LoadedMesh::LoadedMesh()
{

}

LoadedMesh::~LoadedMesh()
{
	if (m_PositionBuffer != nullptr) m_PositionBuffer->Release();
	if (m_UploadPositionBuffer != nullptr) m_UploadPositionBuffer->Release();

	if (m_UvBuffer != nullptr) m_UvBuffer->Release();
	if (m_UploadUvBuffer != nullptr) m_UploadUvBuffer->Release();

	if (m_NormalBuffer != nullptr) m_NormalBuffer->Release();
	if (m_UploadNormalBuffer != nullptr) m_UploadNormalBuffer->Release();

	if (m_TangentBuffer != nullptr) m_TangentBuffer->Release();
	if (m_UploadTangentBuffer != nullptr) m_UploadTangentBuffer->Release();

	if (m_BiTangentBuffer != nullptr) m_BiTangentBuffer->Release();
	if (m_UploadBiTangentBuffer != nullptr) m_UploadBiTangentBuffer->Release();
}

void LoadedMesh::LoadMeshInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File)
{
	fread(&m_VertexCount, sizeof(int), 1, File);

	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(m_MeshName, sizeof(char), WordCount, File);
	m_MeshName[WordCount] = '\0';

	int PositionCount = 0, ColorCount = 0, UvCount = 0, NormalCount = 0, TangentCount = 0, BiTangentCount = 0, SubMeshCount = 0;

	char Word[64] = { '\0' };

	while (true) {
		fread(&WordCount, sizeof(BYTE), 1, File);
		fread(Word, sizeof(char), WordCount, File);
		Word[WordCount] = '\0';

		if (!strcmp(Word, "<Bounds>:")) {
			fread(&m_AabbCenter, sizeof(float), 3, File);
			fread(&m_AAbbExtent, sizeof(float), 3, File);

			// 충돌 처리를 위한 바운딩 박스 설정
			m_BoundingBox.Center = m_AabbCenter;
			m_BoundingBox.Extents = m_AAbbExtent;
		}

		else if (!strcmp(Word, "<Positions>:")) {
			fread(&PositionCount, sizeof(int), 1, File);

			if (PositionCount > 0) {
				m_Position = new DirectX::XMFLOAT3[PositionCount];
				fread(m_Position, sizeof(DirectX::XMFLOAT3), PositionCount, File);

				// Position Buffer 생성
				m_PositionBuffer = CreateBuffer(Device, CommandList, m_Position, sizeof(DirectX::XMFLOAT3) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadPositionBuffer);

				m_PositionBufferView.BufferLocation = m_PositionBuffer->GetGPUVirtualAddress();
				m_PositionBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
				m_PositionBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * m_VertexCount;

				delete[] m_Position;
			}
		}

		else if (!strcmp(Word, "<Colors>:")) {
			fread(&ColorCount, sizeof(int), 1, File);

			if (ColorCount > 0) {
				fread(&m_Color, sizeof(DirectX::XMFLOAT4), ColorCount, File);
			}
		}

		else if (!strcmp(Word, "<TextureCoords0>:")) {
			fread(&UvCount, sizeof(int), 1, File);

			if (UvCount > 0) {
				m_Uv0 = new DirectX::XMFLOAT2[UvCount];
				fread(m_Uv0, sizeof(DirectX::XMFLOAT2), UvCount, File);

				// Uv Buffer 생성
				m_UvBuffer = CreateBuffer(Device, CommandList, m_Uv0, sizeof(DirectX::XMFLOAT2) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadUvBuffer);

				m_UvBufferView.BufferLocation = m_UvBuffer->GetGPUVirtualAddress();
				m_UvBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT2);
				m_UvBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT2) * m_VertexCount;

				delete[] m_Uv0;
			}
		}

		else if (!strcmp(Word, "<TextureCoords1>:")) {
			fread(&UvCount, sizeof(int), 1, File);

			if (UvCount > 0) {
				m_Uv1 = new DirectX::XMFLOAT2[UvCount];
				fread(m_Uv1, sizeof(DirectX::XMFLOAT2), UvCount, File);

				delete[] m_Uv1;
			}
		}

		else if (!strcmp(Word, "<Normals>:")) {
			fread(&NormalCount, sizeof(int), 1, File);

			if (NormalCount > 0) {
				m_Normal = new DirectX::XMFLOAT3[NormalCount];
				fread(m_Normal, sizeof(DirectX::XMFLOAT3), NormalCount, File);

				// Normal Buffer 생성
				m_NormalBuffer = CreateBuffer(Device, CommandList, m_Normal, sizeof(DirectX::XMFLOAT3) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadNormalBuffer);

				m_NormalBufferView.BufferLocation = m_NormalBuffer->GetGPUVirtualAddress();
				m_NormalBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
				m_NormalBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * m_VertexCount;

				delete[] m_Normal;
			}
		}

		else if (!strcmp(Word, "<Tangents>:")) {
			fread(&TangentCount, sizeof(int), 1, File);

			if (TangentCount > 0) {
				m_Tangent = new DirectX::XMFLOAT3[TangentCount];
				fread(m_Tangent, sizeof(DirectX::XMFLOAT3), TangentCount, File);

				// Tangent Buffer 생성
				m_TangentBuffer = CreateBuffer(Device, CommandList, m_Tangent, sizeof(DirectX::XMFLOAT3) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadTangentBuffer);

				m_TangentBufferView.BufferLocation = m_TangentBuffer->GetGPUVirtualAddress();
				m_TangentBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
				m_TangentBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * m_VertexCount;

				delete[] m_Tangent;
			}
		}

		else if (!strcmp(Word, "<BiTangents>:")) {
			fread(&BiTangentCount, sizeof(int), 1, File);

			if (BiTangentCount > 0) {
				m_BiTangent = new DirectX::XMFLOAT3[BiTangentCount];
				fread(m_BiTangent, sizeof(DirectX::XMFLOAT3), BiTangentCount, File);

				// BiTangent Buffer 생성
				m_BiTangentBuffer = CreateBuffer(Device, CommandList, m_BiTangent, sizeof(DirectX::XMFLOAT3) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadBiTangentBuffer);

				m_BiTangentBufferView.BufferLocation = m_BiTangentBuffer->GetGPUVirtualAddress();
				m_BiTangentBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
				m_BiTangentBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * m_VertexCount;

				delete[] m_BiTangent;
			}
		}

		else if (!strcmp(Word, "<SubMeshes>:")) {
			fread(&SubMeshCount, sizeof(int), 1, File);

			if (SubMeshCount > 0) {
				for (int i = 0; i < SubMeshCount; ++i) {
					fread(&WordCount, sizeof(BYTE), 1, File);
					fread(Word, sizeof(char), WordCount, File);
					Word[WordCount] = '\0';

					if (!strcmp(Word, "<SubMesh>:")) {
						int IndexCount = 0;
						fread(&IndexCount, sizeof(int), 1, File);
						fread(&m_IndexCount, sizeof(int), 1, File); // -> SubMesh의 Index Count

						if (m_IndexCount > 0) {
							// Index Buffer를 생성
							m_MeshIndex = new UINT[m_IndexCount];
							fread(m_MeshIndex, sizeof(UINT), m_IndexCount, File);

							m_IndexBuffer = CreateBuffer(Device, CommandList, m_MeshIndex, sizeof(UINT) * m_IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_UploadIndexBuffer);

							m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
							m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
							m_IndexBufferView.SizeInBytes = sizeof(UINT) * m_IndexCount;

							delete[] m_MeshIndex;
						}
					}
				}
			}
		}

		else if (!strcmp(Word, "</Mesh>")) {
			break;
		}
	}
}

void LoadedMesh::UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[5] = { m_PositionBufferView, m_UvBufferView, m_NormalBufferView, m_TangentBufferView, m_BiTangentBufferView };
	CommandList->IASetVertexBuffers(0, 5, VertexBufferView);
}

void LoadedMesh::UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[6] = { m_PositionBufferView, m_UvBufferView, m_NormalBufferView, m_TangentBufferView, m_BiTangentBufferView, InstanceBufferView };
	CommandList->IASetVertexBuffers(0, 6, VertexBufferView);
}

// --------------------
SkinnedMesh::SkinnedMesh()
{

}

SkinnedMesh::~SkinnedMesh()
{
	if (m_BindPoseBoneOffset != nullptr) delete[] m_BindPoseBoneOffset;
	if (m_BoneOffset != nullptr) delete[] m_BoneOffset;
	if (m_BoneOffsetBuffer != nullptr) m_BoneOffsetBuffer->Release();

	if (m_BoneTransform != nullptr) delete[] m_BoneTransform;
	if (m_BoneTransformBuffer != nullptr) m_BoneTransformBuffer->Release();

	if (m_BoneIndexBuffer != nullptr) m_BoneIndexBuffer->Release();
	if (m_UploadBoneIndexBuffer != nullptr) m_UploadBoneIndexBuffer->Release();

	if (m_BoneWeightBuffer != nullptr) m_BoneWeightBuffer->Release();
	if (m_UploadBoneWeightBuffer != nullptr) m_UploadBoneWeightBuffer->Release();

	if (m_BoneName != nullptr) delete[] m_BoneName;
	if (m_BoneFrame != nullptr) delete[] m_BoneFrame;
}

void SkinnedMesh::LoadSkinInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File)
{
	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(m_MeshName, sizeof(char), WordCount, File);
	m_MeshName[WordCount] = '\0';

	char Word[64] = { '\0' };

	while (true) {
		fread(&WordCount, sizeof(BYTE), 1, File);
		fread(Word, sizeof(char), WordCount, File);
		Word[WordCount] = '\0';

		if (!strcmp(Word, "<BonesPerVertex>:")) {
			fread(&m_BonePerVertex, sizeof(int), 1, File);
		}

		else if (!strcmp(Word, "<Bounds>:")) {
			fread(&m_AabbCenter, sizeof(float), 3, File);
			fread(&m_AAbbExtent, sizeof(float), 3, File);
		}

		else if (!strcmp(Word, "<BoneNames>:")) {
			fread(&m_BoneCount, sizeof(int), 1, File);

			if (m_BoneCount > 0) {
				m_BoneName = new char[m_BoneCount][64];
				m_BoneFrame = new GameObject*[m_BoneCount];

				for (int i = 0; i < m_BoneCount; ++i) {
					fread(&WordCount, sizeof(BYTE), 1, File);
					fread(m_BoneName[i], sizeof(char), WordCount, File);
					m_BoneName[i][WordCount] = '\0';
					m_BoneFrame[i] = nullptr;
				}
			}
		}

		else if (!strcmp(Word, "<BoneOffsets>:")) {
			fread(&m_BoneCount, sizeof(int), 1, File);

			if (m_BoneCount > 0) {
				m_BindPoseBoneOffset = new DirectX::XMFLOAT4X4[m_BoneCount];
				fread(m_BindPoseBoneOffset, sizeof(float), 16 * m_BoneCount, File);
			}
		}

		else if (!strcmp(Word, "<BoneWeights>:")) {
			fread(&m_VertexCount, sizeof(int), 1, File);

			if (m_VertexCount > 0) {
				m_BoneIndex = new DirectX::XMUINT4[m_VertexCount];
				m_BoneWeight = new DirectX::XMFLOAT4[m_VertexCount];

				fread(m_BoneIndex, sizeof(DirectX::XMUINT4), m_VertexCount, File);
				fread(m_BoneWeight, sizeof(DirectX::XMFLOAT4), m_VertexCount, File);

				// Bone Index Buffer 생성
				m_BoneIndexBuffer = CreateBuffer(Device, CommandList, m_BoneIndex, sizeof(DirectX::XMUINT4) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadBoneIndexBuffer);

				m_BoneIndexBufferView.BufferLocation = m_BoneIndexBuffer->GetGPUVirtualAddress();
				m_BoneIndexBufferView.StrideInBytes = sizeof(DirectX::XMUINT4);
				m_BoneIndexBufferView.SizeInBytes = sizeof(DirectX::XMUINT4) * m_VertexCount;

				delete[] m_BoneIndex;

				// Bone Weight Buffer 생성
				m_BoneWeightBuffer = CreateBuffer(Device, CommandList, m_BoneWeight, sizeof(DirectX::XMFLOAT4) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadBoneWeightBuffer);

				m_BoneWeightBufferView.BufferLocation = m_BoneWeightBuffer->GetGPUVirtualAddress();
				m_BoneWeightBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT4);
				m_BoneWeightBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT4) * m_VertexCount;

				delete[] m_BoneWeight;
			}
		}

		else if (!strcmp(Word, "</SkinningInfo>")) {
			break;
		}
	}
}

void SkinnedMesh::CreateShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	UINT BufferSize = sizeof(DirectX::XMFLOAT4X4) * 128;

	m_BoneOffsetBuffer = CreateBuffer(Device, CommandList, nullptr, BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_BoneOffsetBuffer->Map(0, nullptr, (void**)&m_BoneOffset);

	m_BoneTransformBuffer = CreateBuffer(Device, CommandList, nullptr, BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_BoneTransformBuffer->Map(0, nullptr, (void**)&m_BoneTransform);
}

void SkinnedMesh::SetBoneFrame(int Index, GameObject* Frame)
{
	m_BoneFrame[Index] = Frame;
}

void SkinnedMesh::UpdateShaderBuffer(ID3D12GraphicsCommandList* CommandList)
{
	if (m_BoneOffsetBuffer != nullptr && m_BoneTransformBuffer != nullptr) {
		for (int i = 0; i < m_BoneCount; ++i) {
			DirectX::XMStoreFloat4x4(&m_BoneOffset[i], DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_BindPoseBoneOffset[i])));
			DirectX::XMStoreFloat4x4(&m_BoneTransform[i], DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_BoneFrame[i]->GetWorldPos())));
		}
		D3D12_GPU_VIRTUAL_ADDRESS BoneOffsetGpuVirtualAddress = m_BoneOffsetBuffer->GetGPUVirtualAddress();
		CommandList->SetGraphicsRootConstantBufferView(5, BoneOffsetGpuVirtualAddress);
		D3D12_GPU_VIRTUAL_ADDRESS BoneTransformGpuVirtualAddress = m_BoneTransformBuffer->GetGPUVirtualAddress();
		CommandList->SetGraphicsRootConstantBufferView(6, BoneTransformGpuVirtualAddress);
	}
}

void SkinnedMesh::UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderBuffer(CommandList);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[7] = { m_PositionBufferView, m_UvBufferView, m_NormalBufferView, m_TangentBufferView, m_BiTangentBufferView, m_BoneIndexBufferView, m_BoneWeightBufferView };
	CommandList->IASetVertexBuffers(0, 7, VertexBufferView);
}

void SkinnedMesh::UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView)
{
	UpdateShaderBuffer(CommandList);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[8] = { m_PositionBufferView, m_UvBufferView, m_NormalBufferView, m_TangentBufferView, m_BiTangentBufferView, m_BoneIndexBufferView, m_BoneWeightBufferView, InstanceBufferView };
	CommandList->IASetVertexBuffers(0, 8, VertexBufferView);
}