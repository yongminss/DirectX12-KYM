#include "stdafx.h"
#include "Skybox.h"

#include "Mesh.h"
#include "Material.h"


Skybox::Skybox(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_SkyboxMesh = new Mesh*[6];

	for (int i = 0; i < 6; ++i) {
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
		UsingMesh = new TextureMesh(Device, CommandList, MeshPosition, i);
		SetMesh(i, UsingMesh);
	}

	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, 1);
	SetMaterial(UsingMaterial);
}

Skybox::~Skybox()
{
	for (int i = 0; i < 6; ++i) if (m_SkyboxMesh[i] != nullptr) delete m_SkyboxMesh[i];
	if (m_SkyboxMesh != nullptr) delete[] m_SkyboxMesh;
}

void Skybox::Animate(float ElapsedTime, DirectX::XMFLOAT3 Position)
{
	SetPosition(Position);
}

void Skybox::Render(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderCode(CommandList);

	if (m_Material != nullptr) m_Material->SetPipeline(CommandList);

	for (int i = 0; i < 6; ++i) {
		if (m_Material != nullptr) m_Material->MappingTexture(CommandList, i);
		if (m_SkyboxMesh[i] != nullptr) m_SkyboxMesh[i]->Render(CommandList);
	}
}