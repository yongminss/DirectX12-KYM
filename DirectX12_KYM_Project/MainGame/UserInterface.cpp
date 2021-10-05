#include "stdafx.h"
#include "UserInterface.h"

#include "Mesh.h"
#include "Material.h"


UserInterface::UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	TextureMesh *UsingMesh = new TextureMesh();
	UsingMesh->CreateMesh(Device, CommandList, DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f), 0);
	SetMesh(UsingMesh);

	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, 2);
	SetMaterial(UsingMaterial);
}

UserInterface::~UserInterface()
{

}