#include "stdafx.h"
#include "Terrain.h"


Terrain::Terrain()
{

}

Terrain::~Terrain()
{
	if (m_HeightMapPos != nullptr) delete[] m_HeightMapPos;
}

void Terrain::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	int Width = 257, Length = 257;

	LoadHeightMapFile(Width, Length);

	TerrainMesh *UsingMesh = new TerrainMesh();
	UsingMesh->CreateMesh(Device, CommandList, DirectX::XMFLOAT3(20.f, 4.f, 20.f), Width, Length, m_HeightMapPos);
	SetMesh(0, UsingMesh);

	TerrainShader *UsingShader = new TerrainShader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	Texture *UsingTexture = new Texture();
	UsingTexture->CreateTexture(Device, CommandList, 6, 3, 2);
	SetTexture(UsingTexture);
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