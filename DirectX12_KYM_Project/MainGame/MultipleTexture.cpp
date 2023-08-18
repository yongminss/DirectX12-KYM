#include "stdafx.h"
#include "MultipleTexture.h"

#include "Mesh.h"
#include "Material.h"


MultipleTexture::MultipleTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_Kind = Kind;

	switch (Kind) {
	case T_SKYBOX:
	{
		m_TextureCount = 6;
		m_MultiMesh = new Mesh*[m_TextureCount];

		for (int i = 0; i < m_TextureCount; ++i) {
			TextureMesh *UsingMesh = nullptr;
			DirectX::XMFLOAT3 MeshPosition{};
			switch (i) {
			case 0:
			{
				MeshPosition = DirectX::XMFLOAT3(+1000.f, +1000.f, +1000.f);
			}
			break;

			case 1:
			{
				MeshPosition = DirectX::XMFLOAT3(+1000.f, +1000.f, -1000.f);
			}
			break;

			case 2:
			{
				MeshPosition = DirectX::XMFLOAT3(-1000.f, +1000.f, +1000.f);
			}
			break;

			case 3:
			{
				MeshPosition = DirectX::XMFLOAT3(+1000.f, +1000.f, +1000.f);
			}
			break;

			case 4:
			{
				MeshPosition = DirectX::XMFLOAT3(+1000.f, +1000.f, +1000.f);
			}
			break;

			case 5:
			{
				MeshPosition = DirectX::XMFLOAT3(+1000.f, -1000.f, +1000.f);
			}
			break;
			}
			UsingMesh = new TextureMesh(Device, CommandList, MeshPosition, DirectX::XMFLOAT2(1.f, 1.f), Kind, i);
			SetMesh(i, UsingMesh);
		}
	}
	break;

	case T_TREE:
	{
		m_TextureCount = 4;
		m_MultiMesh = new Mesh*[m_TextureCount];

		for (int i = 0; i < m_TextureCount; ++i) {
			TextureMesh *UsingMesh = nullptr;
			DirectX::XMFLOAT3 MeshPosition{};
			switch (i) {
			case 0:
			{
				MeshPosition = DirectX::XMFLOAT3(+75.f, +100.f, +0.f);
			}
			break;

			case 1:
			{
				MeshPosition = DirectX::XMFLOAT3(+75.f, +100.f, +0.f);
			}
			break;

			case 2:
			{
				MeshPosition = DirectX::XMFLOAT3(+0.f, +100.f, +75.f);
			}
			break;

			case 3:
			{
				MeshPosition = DirectX::XMFLOAT3(+0.f, +100.f, +75.f);
			}
			break;
			}
			UsingMesh = new TextureMesh(Device, CommandList, MeshPosition, DirectX::XMFLOAT2(1.f, 1.f), Kind, i);
			SetMesh(i, UsingMesh);
		}
	}
	break;

	case T_ITEMHP:
	{
		m_TextureCount = 2;
		m_MultiMesh = new Mesh*[m_TextureCount];

		for (int i = 0; i < m_TextureCount; ++i) {
			TextureMesh *UsingMesh = nullptr;
			DirectX::XMFLOAT3 MeshPosition{};
			switch (i) {
			case 0:
			{
				MeshPosition = DirectX::XMFLOAT3(+5.f, +5.f, +0.f);
			}
			break;

			case 1:
			{
				MeshPosition = DirectX::XMFLOAT3(+5.f, +5.f, +0.f);
			}
			break;
			}
			UsingMesh = new TextureMesh(Device, CommandList, MeshPosition, DirectX::XMFLOAT2(1.f, 1.f), Kind, i);
			SetMesh(i, UsingMesh);
		}
	}
	break;
	}

	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
	SetMaterial(UsingMaterial);
}

MultipleTexture::~MultipleTexture()
{
	for (int i = 0; i < m_TextureCount; ++i) if (m_MultiMesh[i] != nullptr) delete m_MultiMesh[i];
	if (m_MultiMesh != nullptr) delete[] m_MultiMesh;
}

void MultipleTexture::Animate(float ElapsedTime, DirectX::XMFLOAT3 Position)
{
	switch (m_Kind) {
	case T_ITEMHP:
	{
		if (m_ActiveAnimate) {
			m_AnimateTime += ElapsedTime;

			float RotateSpeed = 270.f * ElapsedTime;
			SetRotate(DirectX::XMFLOAT3(0.f, RotateSpeed, 0.f));

			m_TransformPos._42 += ElapsedTime * 20.f;

			if (2.5f < m_AnimateTime) m_ActiveAnimate = false, m_AnimateTime = 0.f, m_TransformPos._41 = 10000.f;
		}
		else {
			float RotateSpeed = 135.f * ElapsedTime;
			SetRotate(DirectX::XMFLOAT3(0.f, RotateSpeed, 0.f));
		}
	}
	break;

	default:
	{
		SetPosition(Position);
	}
	break;
	}
}

void MultipleTexture::Render(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderCode(CommandList);

	if (m_Material != nullptr) m_Material->SetPipeline(CommandList);

	for (int i = 0; i < m_TextureCount; ++i) {
		if (m_Material != nullptr) m_Material->MappingTexture(CommandList, i);
		if (m_MultiMesh[i] != nullptr) m_MultiMesh[i]->Render(CommandList);
	}
}