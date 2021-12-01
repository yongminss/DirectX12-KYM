#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Monster_WeakOrc.bin", true);
	SetChild(Model);

	// Player ������Ʈ�� �ٶ󺸴� ȭ���� �÷��̾�� ������ �� �ְ� Camera ����
	m_Camera = new Camera();
	m_Camera->CreateCamera(Device, CommandList);
}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

void Player::Move(HWND Hwnd, POINT PreviousPos, float MapY)
{
	// ���콺 �Է����� �÷��̾��� ������ ��ȯ
	POINT MousePos{};
	float MouseDistance = 0.f;

	if (GetCapture() == Hwnd) {
		GetCursorPos(&MousePos);
		MouseDistance = (MousePos.x - PreviousPos.x) / 10.f;
		SetCursorPos(PreviousPos.x, PreviousPos.y);

		if (MouseDistance != 0.f) SetRotate(DirectX::XMFLOAT3(0.f, MouseDistance, 0.f));
	}

	// Height Map�� ���̿� ���� �÷��̾��� Y��ǥ�� ����
	float MapDistance = m_TransformPos._42 - MapY;

	if (abs(MapDistance) > 0.5f) {
		DirectX::XMFLOAT3 Position{};
		float Speed = -((MapDistance * 15.f) * m_ElapsedTime);
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetUp()), Speed)));
		SetPosition(Position);
	}

	// �Է��� ȣ��Ǹ� Player�� �ӵ��� õõ�� ����
	if (m_ActiveMove[0] == true || m_ActiveMove[1] == true || m_ActiveMove[2] == true || m_ActiveMove[3] == true) {
		if (m_Speed <= 150.f) m_Speed += m_ElapsedTime * 100.f;
	}

	// 1. �Է��� ������ �ӵ��� ����
	// 2. ���� or �¿� Ű�� ���ÿ� �ԷµǸ� �ӵ��� ����
	if ((m_ActiveMove[0] == false && m_ActiveMove[1] == false && m_ActiveMove[2] == false && m_ActiveMove[3] == false) ||
		(m_ActiveMove[0] == true && m_ActiveMove[1] == true) ||
		(m_ActiveMove[2] == true && m_ActiveMove[3] == true)) {
		m_Speed -= m_ElapsedTime * 500.f;
		// �ӵ��� �����ϴٰ� 0���� �۾����� ������ ����
		if (m_Speed <= 0.f) m_Speed = 0.f;
	}

	// m_Speed�� 0�� �ƴϸ� �����̰� �ִ� �����̹Ƿ� Move ����
	if (m_Speed != 0) {
		DirectX::XMFLOAT3 Position{};
		float Speed = m_Speed * m_ElapsedTime;

		// �ӵ��� ���� �ִϸ��̼� ����
		if (m_Speed >= 80.f) SetAnimationTrackIndex(2);
		else SetAnimationTrackIndex(1);

		// Set Position - ActiveMove : 0(��), 1(��), 2(��), 3(��)
		if (m_ActiveMove[0] == true) {
			DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), +Speed)));
			m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;
		}
		if (m_ActiveMove[1] == true) {
			DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), -Speed)));
			m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;
		}
		if (m_ActiveMove[2] == true) {
			DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetRight()), -Speed)));
			m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;
		}
		if (m_ActiveMove[3] == true) {
			DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetRight()), +Speed)));
			m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;
		}
	}
	// m_Speed�� 0�̸� �������� �����Ƿ� IDLE ���·� ����
	else {
		// Walk or Run �ִϸ��̼��� ��쿡�� IDEL�� ����
		if (GetActiveAniTrackIndex() == 1 || GetActiveAniTrackIndex() == 2)
			SetAnimationTrackIndex(0);
	}

	UpdateTransform(nullptr);
}

void Player::Animate(float ElapsedTime, HWND Hwnd, POINT PreviousPos, float MapY)
{
	m_ElapsedTime = ElapsedTime;

	Move(Hwnd, PreviousPos, MapY);

	GameObject::Animate(ElapsedTime);
}

void Player::UpdateCamera(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, m_ElapsedTime, this);
}