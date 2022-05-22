#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_PreviousCameraTransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Player_Soldier.bin", nullptr, true);
	SetChild(Model);

	// Player ������Ʈ�� �ٶ󺸴� ȭ���� �÷��̾�� ������ �� �ְ� Camera ����
	m_Camera = new Camera();
	m_Camera->CreateCamera(Device, CommandList);

	// �÷��̾ ũ�⸦ ��������Ƿ� ī�޶� ��ǥ�� ���
	m_PreviousCameraTransformPos = m_TransformPos;
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
	float MapDistance = (m_TransformPos._42 - 15.f) - MapY;

	if (abs(MapDistance) > 0.05f) {
		DirectX::XMFLOAT3 Position{};
		float Speed = -((MapDistance * 15.f) * m_ElapsedTime);
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetUp()), Speed)));
		SetPosition(Position);
	}

	if (true == GetChangeState()) {
		SetChangeState(false), m_State = STATE_NONE;
	}

	// Ű���� or ���콺 ��ɿ� ���� �÷��̾ ������ �ִϸ��̼��� ����
	switch (m_State) {
	case STATE_ROLL:
	{
		if (GetCurrentAnimationTrackIndex() != P_ROLL) SetAnimationTrack(P_ROLL, ANIMATION_TYPE_ONCE);

		float RollSpeed = m_ElapsedTime * 250.f;
		DirectX::XMFLOAT3 Position{};
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), RollSpeed)));
		m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;

		m_PreviousCameraTransformPos._41 = m_TransformPos._41, m_PreviousCameraTransformPos._42 = m_TransformPos._42, m_PreviousCameraTransformPos._43 = m_TransformPos._43;

		m_RollDistance += RollSpeed;

		// ���� �Ÿ��� �̵������� ������ ����
		if (m_RollDistance >= 200.f) m_State = STATE_NONE, m_RollDistance = 0.f;
	}
	break;
	}

	// �Է��� ȣ��Ǹ� Player�� �ӵ��� õõ�� ����
	if (m_ActiveMove[0] == true || m_ActiveMove[1] == true || m_ActiveMove[2] == true || m_ActiveMove[3] == true) {
		if (m_Speed <= 150.f) m_Speed += m_ElapsedTime * 500.f;
	}

	// 1. �Է��� ������ �ӵ��� ����
	// 2. ���� or �¿� Ű�� ���ÿ� �ԷµǸ� �ӵ��� ����
	if ((m_ActiveMove[0] == false && m_ActiveMove[1] == false && m_ActiveMove[2] == false && m_ActiveMove[3] == false) ||
		(m_ActiveMove[0] == true && m_ActiveMove[1] == true) ||
		(m_ActiveMove[2] == true && m_ActiveMove[3] == true)) {
		m_Speed -= m_ElapsedTime * 1000.f;
		// �ӵ��� �����ϴٰ� 0���� �۾����� ������ ����
		if (m_Speed <= 0.f) m_Speed = 0.f;
	}

	// m_Speed�� 0�� �ƴϸ� �����̰� �ִ� �����̹Ƿ� Move ����
	if (m_Speed != 0 && STATE_ROLL != m_State) {
		DirectX::XMFLOAT3 Position{};
		float Speed = m_Speed * m_ElapsedTime;

		switch (m_State) {
		case STATE_NONE:
		{
			// ���� �̵�
			if (true == m_ActiveMove[0]) {
				SetAnimationTrack(P_RUN_FRONT, ANIMATION_TYPE_LOOP);
				// ���� + ��
				if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_FRONT_LEFT, ANIMATION_TYPE_LOOP);
				// ���� + ��
				if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_FRONT_RIGHT, ANIMATION_TYPE_LOOP);
			}
			// �Ĺ� �̵�
			else if (true == m_ActiveMove[1]) {
				SetAnimationTrack(P_RUN_BACK, ANIMATION_TYPE_LOOP);
				// �Ĺ� + ��
				if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_BACK_LEFT, ANIMATION_TYPE_LOOP);
				// �Ĺ� + ��
				if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_BACK_RIGHT, ANIMATION_TYPE_LOOP);
			}
			// ���� �̵�
			else if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_LEFT, ANIMATION_TYPE_LOOP);
			// ���� �̵�
			else if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_RIGHT, ANIMATION_TYPE_LOOP);
		}
		break;

		case STATE_SHOOT: // �����̴� ���¿��� ���� �ִϸ��̼��� ����
		{
			// 1. Shoot or Reload Animation�� ���� ������ ��ȣ�ۿ� �߻� - �ݺ��� �ƴ� �� ���� ���ุ �ؾ� �� (Type - Convert)
			// 2. ���� �����ӿ��� 10��° Idle_Shoot�� ����Ǵ� �߿� �÷��̾� �̵��� �߻��ϸ� 11��° Run_Shoot���� �ִϸ��̼��� �̾ �����ϰ� ����
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			// ���� �������� �ִϸ��̼ǰ� ���� ������ �ִϸ��̼��� ��ȣ�� ���Ͽ� ��ȯ ���θ� ����
			if (P_SHOOT <= NowAnimationIndex && P_RELOAD > NowAnimationIndex) AnimateConversion = true;

			// ���� �̵�
			if (m_ActiveMove[0]) {
				// ���� + ��
				if (m_ActiveMove[2]) {
					if (P_RUN_SHOOT_FRONT_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// ���� + ��
				else if (m_ActiveMove[3]) {
					if (P_RUN_SHOOT_FRONT_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// ����
				else {
					if (P_RUN_SHOOT_FRONT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// �Ĺ� �̵�
			else if (m_ActiveMove[1]) {
				// �Ĺ� + ��
				if (m_ActiveMove[2]) {
					if (P_RUN_SHOOT_BACK_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// �Ĺ� + ��
				else if (m_ActiveMove[3]) {
					if (P_RUN_SHOOT_BACK_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// �Ĺ�
				else {
					if (P_RUN_SHOOT_BACK != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// ���� �̵�
			else if (m_ActiveMove[2]) {
				if (P_RUN_SHOOT_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
			// ���� �̵�
			else if (m_ActiveMove[3]) {
				if (P_RUN_SHOOT_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
		}
		break;

		case STATE_RELOAD: // Shoot�� ���������� ��ȯ�� ���� ó���� ����
		{
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			if (P_RELOAD <= NowAnimationIndex && P_ROLL > NowAnimationIndex) AnimateConversion = true;

			// ��
			if (true == m_ActiveMove[0]) {
				if (true == m_ActiveMove[2]) {
					if (P_RUN_RELOAD_FRONT_LEFT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_FRONT_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				else if (true == m_ActiveMove[3]) {
					if (P_RUN_RELOAD_FRONT_RIGHT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_FRONT_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				else {
					if (P_RUN_RELOAD_FRONT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_FRONT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// ��
			else if (true == m_ActiveMove[1]) {
				if (true == m_ActiveMove[2]) {
					if (P_RUN_RELOAD_BACK_LEFT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_BACK_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				else if (true == m_ActiveMove[3]) {
					if (P_RUN_RELOAD_BACK_RIGHT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_BACK_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				else {
					if (P_RUN_RELOAD_BACK != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_BACK, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// ��
			else if (true == m_ActiveMove[2]) {
				if (P_RUN_RELOAD_LEFT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
			// ��
			else if (true == m_ActiveMove[3]) {
				if (P_RUN_RELOAD_RIGHT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
		}
		break;
		}

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
	// �������� �ʴ� ������ ���� �ִϸ��̼��� ����
	else {
		switch (m_State) {
		case STATE_NONE:
		{
			SetAnimationTrack(P_IDLE, ANIMATION_TYPE_LOOP);
		}
		break;

		case STATE_SHOOT:
		{
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			if (P_SHOOT <= NowAnimationIndex && P_RELOAD > NowAnimationIndex) AnimateConversion = true;

			if (P_SHOOT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_SHOOT, ANIMATION_TYPE_CONVERT, AnimateConversion);
		}
		break;

		case STATE_RELOAD:
		{
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			if (P_RELOAD <= NowAnimationIndex && P_ROLL > NowAnimationIndex) AnimateConversion = true;

			if (P_RELOAD != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RELOAD, ANIMATION_TYPE_CONVERT, AnimateConversion);
		}
		break;
		}
	}
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