#include "stdafx.h"
#include "UserInterface.h"


UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{

}

void UserInterface::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	TextureMesh *UsingMesh = new TextureMesh();
	UsingMesh->CreateMesh(Device, CommandList, DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f), 0);
	SetMesh(0, UsingMesh);

	UserInterfaceShader *UsingShader = new UserInterfaceShader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	Texture *UsingTexture = new Texture();
	UsingTexture->CreateTexture(Device, CommandList, 0, 2, 1);
	SetTexture(UsingTexture);
}