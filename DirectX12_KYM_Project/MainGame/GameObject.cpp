#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	if (m_Mesh != nullptr) delete m_Mesh;
	if (m_Shader != nullptr) delete m_Shader;
}

void GameObject::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	// 오브젝트의 정점들의 집합인 Mesh 생성
	m_Mesh = new Mesh();
	m_Mesh->CreateMesh(Device, CommandList);

	// 오브젝트가 사용할 그래픽스 파이프라인을 생성
	m_Shader = new Shader();
	m_Shader->CreateShader(Device, RootSignature);
}

void GameObject::UpdateShaderCode(ID3D12GraphicsCommandList* CommandList)
{
	DirectX::XMFLOAT4X4 worldpos{};
	DirectX::XMStoreFloat4x4(&worldpos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_WorldPos)));
	CommandList->SetGraphicsRoot32BitConstants(1, 16, &worldpos, 0);
}

void GameObject::Render(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	if (m_Mesh != nullptr) m_Mesh->Render(CommandList);
}