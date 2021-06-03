#include "stdafx.h"
#include "Player.h"


Player::Player()
{

}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

void Player::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	Mesh *UsingMesh = new Mesh();
	UsingMesh->CreateMesh(Device, CommandList, 10.f);
	SetMesh(0, UsingMesh);

	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	// Player 오브젝트가 바라보는 화면을 플레이어에게 보여줄 수 있게 Camera 생성
	m_Camera = new Camera();
	m_Camera->CreateCamera();
}

void Player::Move(int Index, float Distance)
{
	DirectX::XMFLOAT3 Position{};

	switch (Index) {
	case 0:
	case 1:
	{
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), Distance)));
	}
	break;

	case 2:
	case 3:
	{
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetRight()), Distance)));
	}
	break;
	}
	m_WorldPos._41 = Position.x, m_WorldPos._42 = Position.y, m_WorldPos._43 = Position.z;
}

void Player::Animate(float ElapsedTime)
{
	if (m_ActiveMove[0] == true) Move(0, +100.f * ElapsedTime);
	if (m_ActiveMove[1] == true) Move(1, -100.f * ElapsedTime);
	if (m_ActiveMove[2] == true) Move(2, -100.f * ElapsedTime);
	if (m_ActiveMove[3] == true) Move(3, +100.f * ElapsedTime);
}

void Player::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, this);

	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	for (int i = 0; i < m_MeshCount; ++i) {
		if (m_Texture != nullptr) m_Texture->Render(CommandList, i);
		if (m_Mesh[i] != nullptr) m_Mesh[i]->Render(CommandList);
	}
}