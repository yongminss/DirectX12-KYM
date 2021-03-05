#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

void GameObject::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	m_Mesh = new Mesh();
	m_Mesh->CreateMesh(Device, CommandList);

	m_Shader = new Shader();
	m_Shader->CreateShader(Device, RootSignature);
}

void GameObject::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Shader->Render(CommandList);

	m_Mesh->Render(CommandList);
}