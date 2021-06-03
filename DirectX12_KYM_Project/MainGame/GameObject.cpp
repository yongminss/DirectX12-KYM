#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	for (int i = 0; i < m_MeshCount; ++i) if (m_Mesh[i] != nullptr) delete m_Mesh[i];
	delete[] m_Mesh;
	if (m_Shader != nullptr) delete m_Shader;
	if (m_Texture != nullptr) delete m_Texture;
}

void GameObject::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	// 오브젝트의 정점들의 집합인 Mesh 생성
	Mesh *UsingMesh = new Mesh();
	UsingMesh->CreateMesh(Device, CommandList, 10.f);
	SetMesh(0, UsingMesh);

	// 오브젝트가 사용할 그래픽스 파이프라인을 생성
	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);
}

void GameObject::SetMesh(int MeshIndex, Mesh* ObjectMesh)
{
	m_Mesh[MeshIndex] = ObjectMesh;
}

void GameObject::SetShader(Shader* ObjectShader)
{
	m_Shader = ObjectShader;
}

void GameObject::SetTexture(Texture* ObjectTexture)
{
	m_Texture = ObjectTexture;
}

void GameObject::SetRight(DirectX::XMFLOAT3 Right)
{
	m_WorldPos._11 = Right.x;
	m_WorldPos._12 = Right.y;
	m_WorldPos._13 = Right.z;
}

void GameObject::SetUp(DirectX::XMFLOAT3 Up)
{
	m_WorldPos._21 = Up.x;
	m_WorldPos._22 = Up.y;
	m_WorldPos._23 = Up.z;
}

void GameObject::SetLook(DirectX::XMFLOAT3 Look)
{
	m_WorldPos._31 = Look.x;
	m_WorldPos._32 = Look.y;
	m_WorldPos._33 = Look.z;
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

void GameObject::Rotate(DirectX::XMFLOAT3 Angle)
{
	DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Angle.x), DirectX::XMConvertToRadians(Angle.y), DirectX::XMConvertToRadians(Angle.z));
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixMultiply(Rotate, DirectX::XMLoadFloat4x4(&m_WorldPos)));
}

void GameObject::Animate(float ElapsedTime)
{

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
	for (int i = 0; i < m_MeshCount; ++i) {
		if (m_Texture != nullptr) m_Texture->Render(CommandList, i);
		if (m_Mesh[i] != nullptr) m_Mesh[i]->Render(CommandList);
	}
}