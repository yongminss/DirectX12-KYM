#include "stdafx.h"
#include "UserInterface.h"

#include "Mesh.h"
#include "Material.h"


UserInterface::UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind, int NumbersIndex)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_Kind = Kind;

	switch (Kind)
	{
	case T_TITLESCREEN:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(1.f, 1.f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	case T_SELECTION:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.5f, 0.1f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	case T_HPBAR:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.42f, 0.061f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	case T_HPGAUGE:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.41f, 0.06f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	case T_AIM:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.06f, 0.08f, 0.f), 0);
		SetMesh(UsingMesh);
	}
	break;

	case T_NUMBERS:
	{
		m_Active = false;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.075f, 0.1f, 0.f), 6, NumbersIndex);
		SetMesh(UsingMesh);
	}
	break;

	case T_GAMEOVER:
	{
		m_Active = false;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(1.f, 1.f, 0.f), 0);
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

void UserInterface::Animate(float ElapsedTime, int Hp)
{
	switch (m_Kind) {
	case T_HPGAUGE:
	{
		// 플레이어의 체력에 비례하여 게이지를 설정
		m_TransformPos._11 = (float)Hp / 100.f, m_TransformPos._41 = ((float)(Hp - 100)) / 240.f - 0.5f;
	}
	break;

	case T_NUMBERS:
	{
		m_Active = Hp;
	}
	break;

	case T_GAMEOVER:
	{
		m_GameOverTime += ElapsedTime;
		int ScreenBrightness = 500 + int(m_GameOverTime * 100.f);
		SetDamaged(ScreenBrightness);
	}
	break;
	}

	UpdateTransform(nullptr);
}

void UserInterface::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (m_Active == true) GameObject::Render(CommandList);
}