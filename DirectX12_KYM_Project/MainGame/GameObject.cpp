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

void GameObject::SetPosition(DirectX::XMFLOAT3 Position)
{
	m_WorldPos._41 = Position.x;
	m_WorldPos._42 = Position.y;
	m_WorldPos._43 = Position.z;
}

DirectX::XMFLOAT3 GameObject::GetRight()
{
	return DirectX::XMFLOAT3(m_WorldPos._11, m_WorldPos._12, m_WorldPos._13);
}

DirectX::XMFLOAT3 GameObject::GetUp()
{
	return DirectX::XMFLOAT3(m_WorldPos._21, m_WorldPos._22, m_WorldPos._23);
}

DirectX::XMFLOAT3 GameObject::GetLook()
{
	return DirectX::XMFLOAT3(m_WorldPos._31, m_WorldPos._32, m_WorldPos._33);
}

DirectX::XMFLOAT3 GameObject::GetPosition()
{
	return DirectX::XMFLOAT3(m_WorldPos._41, m_WorldPos._42, m_WorldPos._43);
}

void GameObject::MoveForward()
{
	m_WorldPos._43 += 50.f;
}

void GameObject::MoveBackward()
{
	m_WorldPos._43 -= 50.f;
}

void GameObject::MoveLeft()
{
	m_WorldPos._41 -= 50.f;
}

void GameObject::MoveRight()
{
	m_WorldPos._41 += 50.f;
}

void GameObject::UpdateShaderCode(ID3D12GraphicsCommandList* CommandList)
{
	DirectX::XMFLOAT4X4 WorldPos{};

	DirectX::XMStoreFloat4x4(&WorldPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_WorldPos)));
	CommandList->SetGraphicsRoot32BitConstants(1, 16, &WorldPos, 0);
}

void GameObject::Render(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	if (m_Mesh != nullptr) m_Mesh->Render(CommandList);
}