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
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Monster_WeakOrc.bin");
	SetChild(Model);

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
	m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;

	UpdateTransform(nullptr);
}

void Player::Animate(float ElapsedTime)
{
	m_ElapsedTime = ElapsedTime;

	if (m_ActiveMove[0] == true) Move(0, +100.f * ElapsedTime);
	if (m_ActiveMove[1] == true) Move(1, -100.f * ElapsedTime);
	if (m_ActiveMove[2] == true) Move(2, -100.f * ElapsedTime);
	if (m_ActiveMove[3] == true) Move(3, +100.f * ElapsedTime);
}

void Player::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, m_ElapsedTime, this);

	GameObject::Render(CommandList);
}