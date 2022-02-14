#include "stdafx.h"
#include "Terrain.h"

#include "Mesh.h"
#include "Material.h"


Terrain::Terrain(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	int Width = 257, Length = 257;

	LoadHeightMapFile(Width, Length);

	TerrainMesh *UsingMesh = new TerrainMesh(Device, CommandList, DirectX::XMFLOAT3(MAP_SCALE, 2.f, MAP_SCALE), Width, Length, m_HeightMapPos);
	SetMesh(UsingMesh);

	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, T_TERRAIN);
	SetMaterial(UsingMaterial);
}

Terrain::~Terrain()
{
	if (m_HeightMapPos != nullptr) delete[] m_HeightMapPos;
}

void Terrain::LoadHeightMapFile(int Width, int Length)
{
	HANDLE HeightMapFile = CreateFile(L"Map/HeightMap.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	BYTE *HeightMapPixel = new BYTE[Width * Length];
	DWORD ByteRead{};
	ReadFile(HeightMapFile, HeightMapPixel, Width * Length, &ByteRead, NULL);
	CloseHandle(HeightMapFile);

	m_HeightMapPos = new BYTE[Width * Length];

	for (int i = 0; i < Length; ++i) {
		for (int j = 0; j < Width; ++j) {
			m_HeightMapPos[i + (Length - 1 - j) * Width] = HeightMapPixel[i + (j * Width)];
		}
	}
	delete[] HeightMapPixel;
}

float Terrain::GetHeightMapYPos(int x, int z)
{
	TerrainMesh *UsingMesh = (TerrainMesh*)m_Mesh;

	return UsingMesh->GetHeightMapYPos(x, z);
}