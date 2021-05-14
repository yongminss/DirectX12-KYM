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

	LoadHeightMapFile();

	TerrainMesh *UsingMesh = new TerrainMesh();
	UsingMesh->CreateMesh(Device, CommandList, 1.f, m_HeightMapPos);
	SetMesh(UsingMesh);

	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);
}

void Terrain::LoadHeightMapFile()
{
	HANDLE HeightMapFile = CreateFile(L"HeightMap.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	m_HeightMapPos = new BYTE[257 * 257];
	DWORD ByteRead{};

	ReadFile(HeightMapFile, m_HeightMapPos, 257 * 257, &ByteRead, NULL);

	CloseHandle(HeightMapFile);
}