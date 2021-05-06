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

	TextureMesh *UsingMesh = new TextureMesh();
	UsingMesh->CreateMesh(Device, CommandList, 0.25f);
	SetMesh(UsingMesh);

	TextureShader *UsingShader = new TextureShader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	Texture *UsingTexture = new Texture();
	UsingTexture->CreateTexture(Device, CommandList, 0);
	SetTexture(UsingTexture);
}