#include "stdafx.h"
#include "UserInterface.h"

#include "Mesh.h"
#include "Material.h"


UserInterface::UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_Kind = Kind;

	switch (Kind)
	{
	case T_TITLESCREEN:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(1.f, 1.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_SELECTION:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.275f, 0.1f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GAMEMANUAL:
	{
		m_Active = false;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.75f, 0.75f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_HPBAR:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.42f, 0.061f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_HPGAUGE:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.41f, 0.06f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_AIM:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.06f, 0.08f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_NUMBERS:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.075f, 0.1f, 0.f), DirectX::XMFLOAT2(0.25f, 0.32f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GAMEOVER:
	{
		m_Active = false;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(1.f, 1.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_ENTERFIRE:
	case T_ENTERMONSTER:
	{
		m_Active = false;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.35f, 0.05f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GUIDEAREA:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.15f, 0.1f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GUIDENONE:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.15f, 0.085f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GUIDEFIRE:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.125f, 0.075f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	case T_GUIDEMONSTER:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.225f, 0.085f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);
	}
	break;

	default:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(0.5f, 0.5f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
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
		float ShiftX = 0.f, ShiftY = 0.f;

		switch (int(ElapsedTime)) {
		case 0:
		{
			int BulletCountTen = Hp / 10;

			for (int i = 0; i < 4; ++i) if (BulletCountTen == i) ShiftX = 0.25f * i;
		}
		break;

		case 1:
		{
			int BulletCountOne = Hp % 10;

			for (int y = 0, i = 0; y < 3; ++y)
				for (int x = 0; x < 4; ++x, ++i)
					if (BulletCountOne == i) ShiftX = 0.25f * x, ShiftY = 0.32f * y;
		}
		break;
		}
		SetChangeTexcoords(DirectX::XMFLOAT4(ShiftX, ShiftY, 0.f, 0.f));
	}
	break;

	case T_GAMEOVER:
	{
		m_Active = true;

		if (5.f > m_AnimateTime) {
			m_AnimateTime += ElapsedTime;
			int ScreenBrightness = 500 + int(m_AnimateTime * 100.f);
			SetChangeTexcoords(DirectX::XMFLOAT4(ScreenBrightness, 0.f, 0.f, 0.f));
		}
	}
	break;

	case T_ENTERFIRE:
	case T_ENTERMONSTER:
	{
		switch (Hp) {
		case 0:
		{
			m_Active = false;
			m_AnimateTime = 0.f;
		}
		break;

		case 1:
		{
			if (m_AnimateTime > 5.f) m_Active = false;
			else {
				m_Active = true;
				m_AnimateTime += ElapsedTime;
			}
		}
		break;
		}
	}
	break;
	}

	UpdateTransform(nullptr);
}

void UserInterface::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (m_Active == true) GameObject::Render(CommandList);
}