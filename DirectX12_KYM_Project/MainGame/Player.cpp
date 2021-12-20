#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_PreviousCameraTransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Monster_WeakOrc.bin", true);
	SetChild(Model);

	// Player ������Ʈ�� �ٶ󺸴� ȭ���� �÷��̾�� ������ �� �ְ� Camera ����
	m_Camera = new Camera();
	m_Camera->CreateCamera(Device, CommandList);

	// �÷��̾ ũ�⸦ ��������Ƿ� ī�޶� ��ǥ�� ���
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	DirectX::XMStoreFloat4x4(&m_PreviousCameraTransformPos, DirectX::XMMatrixMultiply(Scale, DirectX::XMLoadFloat4x4(&m_PreviousCameraTransformPos)));
}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

DirectX::XMFLOAT3 Player::GetCameraWorldRight()
{
	if (m_Camera != nullptr) return m_Camera->GetRight();
}

DirectX::XMFLOAT3 Player::GetCameraWorldUp()
{
	if (m_Camera != nullptr) return m_Camera->GetUp();
}

DirectX::XMFLOAT3 Player::GetCameraWorldLook()
{
	if (m_Camera != nullptr) return m_Camera->GetLook();
}

DirectX::XMFLOAT3 Player::GetCameraWorldPosition()
{
	if (m_Camera != nullptr) return m_Camera->GetPosition();
}

void Player::Move(HWND Hwnd, POINT PreviousPos, float MapY)
{
	// ���콺 �Է����� �÷��̾��� ������ ��ȯ
	POINT MousePos{};
	float YAxisRotation = 0.f, XAxisRotation = 0.f;

	if (GetCapture() == Hwnd) {
		GetCursorPos(&MousePos);
		YAxisRotation = (MousePos.x - PreviousPos.x) / 10.f;
		XAxisRotation = (MousePos.y - PreviousPos.y) / 10.f;
		SetCursorPos(PreviousPos.x, PreviousPos.y);

		// ���콺 �Է��� ������ �÷��̾�� ī�޶� ȸ��
		if (YAxisRotation != 0.f || XAxisRotation != 0.f) {
			// ī�޶��� X�� ȸ���� ���� ������ ȸ������ ���� - �÷��̾�� X�� ȸ���� ���� ����
			m_Pitch += XAxisRotation;

			// ī�޶��� ������ ������ ���� �ִ� Pitch ���� ����
			if (m_Pitch >= 80.f || m_Pitch <= -50.f) {
				m_Pitch -= XAxisRotation;
			}
			// X�� ȸ���� �ִ뿡 �������� �ʾ����� ȸ���� ����
			else {
				// ���콺�� �Էµ� ȸ������ �������� X or Y�� ȸ���� ����
				if ((XAxisRotation > -0.2f || XAxisRotation < 0.2f) && (YAxisRotation > 0.5f || YAxisRotation < -0.5f)) {
					// ���콺�� ���Ʒ� �������� ���� �¿��� �������� ũ�ٸ� �÷��̾ Y�� ȸ��
					SetRotate(DirectX::XMFLOAT3(0.f, YAxisRotation, 0.f));
					m_PreviousCameraTransformPos = m_TransformPos;
					m_Pitch = 0.f;
				}
				else {
					// �÷��̾�� �״��, ī�޶� X�� ȸ��
					DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(XAxisRotation), DirectX::XMConvertToRadians(0.f), DirectX::XMConvertToRadians(0.f));
					DirectX::XMStoreFloat4x4(&m_PreviousCameraTransformPos, DirectX::XMMatrixMultiply(Rotate, DirectX::XMLoadFloat4x4(&m_PreviousCameraTransformPos)));
				}
			}
		}
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
		if (m_Speed >= 80.f) SetAnimationTrack(P_RUN, ANIMATION_TYPE_LOOP);
		else SetAnimationTrack(P_WALK, ANIMATION_TYPE_LOOP);

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
		// ȸ�� ���ο� ���� ���� ī�޶��� ���� ��ǥ�� �̵��ؾ��ϹǷ� �÷��̾��� ���� ��ǥ�� ����
		m_PreviousCameraTransformPos._41 = m_TransformPos._41, m_PreviousCameraTransformPos._42 = m_TransformPos._42, m_PreviousCameraTransformPos._43 = m_TransformPos._43;
	}
	// m_Speed�� 0�̸� �������� �����Ƿ� IDLE ���·� ����
	else {
		// Walk or Run �ִϸ��̼��� ��쿡�� IDEL�� ����
		if (GetCurrentAnimationTrackIndex() == P_WALK || GetCurrentAnimationTrackIndex() == P_RUN)
			SetAnimationTrack(P_IDLE, ANIMATION_TYPE_LOOP);
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