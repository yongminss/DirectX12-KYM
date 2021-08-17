#include "stdafx.h"
#include "Skybox.h"


Skybox::Skybox()
{

}

Skybox::~Skybox()
{
	for (int i = 0; i < 6; ++i) if (m_SkyboxMesh[i] != nullptr) delete m_SkyboxMesh[i];
	if (m_SkyboxMesh != nullptr) delete[] m_SkyboxMesh;
}

void Skybox::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_SkyboxMesh = new Mesh*[6];

	for (int i = 0; i < 6; ++i) {
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

	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, 1);
	SetMaterial(UsingMaterial);
}

void Skybox::Animate(float ElapsedTime, DirectX::XMFLOAT3 Position)
{
	SetPosition(Position);

	UpdateTransform(nullptr);
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