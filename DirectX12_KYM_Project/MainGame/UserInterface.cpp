#include "stdafx.h"
#include "UserInterface.h"

#include "Mesh.h"
#include "Material.h"


UserInterface::UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	switch (Kind)
	{
	case 2:
	case 3:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.4f, 0.06f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	default:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.5f, 0.5f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;
	}
	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
	SetMaterial(UsingMaterial);
}

UserInterface::~UserInterface()
{

}

void UserInterface::Animate(float ElapsedTime)
{
	float NextFrameBarxSize = m_TransformPos._11 - (0.5f * ElapsedTime);
	float BarxPosition = m_TransformPos._41 - (0.199f * ElapsedTime);

	m_TransformPos._11 = NextFrameBarxSize;
	m_TransformPos._41 = BarxPosition;

	// 체력 게이지가 0이 됐으면 초기화
	if (NextFrameBarxSize < 0.f) {
		m_TransformPos._11 = 1.f, m_TransformPos._41 = -0.51f;
	}

	UpdateTransform(nullptr);
}