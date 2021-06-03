#include "stdafx.h"
#include "Skybox.h"


Skybox::Skybox()
{

}

Skybox::~Skybox()
{

}

void Skybox::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 6;
	m_Mesh = new Mesh*[m_MeshCount];

	for (int i = 0; i < m_MeshCount; ++i) {
		TextureMesh *UsingMesh = new TextureMesh();
		DirectX::XMFLOAT3 MeshPosition{};
		switch (i) {
		case 0:
			MeshPosition.x = 500.f, MeshPosition.y = 500.f, MeshPosition.z = +500.f;
			break;

		case 1:
			MeshPosition.x = 500.f, MeshPosition.y = 500.f, MeshPosition.z = -500.f;
			break;

		case 2:
			MeshPosition.x = -500.f, MeshPosition.y = 500.f, MeshPosition.z = 500.f;
			break;

		case 3:
			MeshPosition.x = +500.f, MeshPosition.y = 500.f, MeshPosition.z = 500.f;
			break;

		case 4:
			MeshPosition.x = 500.f, MeshPosition.y = +500.f, MeshPosition.z = 500.f;
			break;

		case 5:
			MeshPosition.x = 500.f, MeshPosition.y = -500.f, MeshPosition.z = 500.f;
			break;
		}
		UsingMesh->CreateMesh(Device, CommandList, MeshPosition, i);
		SetMesh(i, UsingMesh);
	}

	SkyboxShader *UsingShader = new SkyboxShader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	Texture *UsingTexture = new Texture();
	UsingTexture->CreateTexture(Device, CommandList, 0, 6);
	SetTexture(UsingTexture);
}

void Skybox::Animate(float ElapsedTime, DirectX::XMFLOAT3 Position)
{
	SetPosition(Position);
}