#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Player_Soldier.bin", nullptr, true);
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
	float YAxisRotation = 0.f, XAxisRotation = 0.f;
	
	if (GetCapture() == Hwnd) {
		GetCursorPos(&MousePos);
		YAxisRotation = (MousePos.x - PreviousPos.x) / 10.f;
		XAxisRotation = (MousePos.y - PreviousPos.y) / 10.f;
		SetCursorPos(PreviousPos.x, PreviousPos.y);

		if (0.f != XAxisRotation) {
			m_Pitch += XAxisRotation;

			if (30.f < m_Pitch) { XAxisRotation = 0.f, m_Pitch = 30.f; }
			if (-20.f > m_Pitch) { XAxisRotation = 0.f, m_Pitch = -20.f; }
		}

		// ���콺 �Է��� ������ �÷��̾�� ī�޶��� ȸ��
		if (0.f != YAxisRotation || 0.f != XAxisRotation) {
			// ���콺�� x������ �̵��ϸ� �÷��̾��� y���� ȸ��
			if (0.f != YAxisRotation) {
				DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMConvertToRadians(YAxisRotation));

				DirectX::XMFLOAT3 Look{};
				DirectX::XMStoreFloat3(&Look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&GetLook()), Rotate));
				SetLook(Look);

				DirectX::XMFLOAT3 Right{};
				DirectX::XMStoreFloat3(&Right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&GetRight()), Rotate));
				SetRight(Right);
			}

			// �÷��̾��� ȸ���� ��������Ƿ� ī�޶��� ȸ�� ����
			if (0.f != YAxisRotation) {
				DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMConvertToRadians(YAxisRotation));

				DirectX::XMStoreFloat3(&m_CameraLook, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_CameraLook), Rotate));

				DirectX::XMStoreFloat3(&m_CameraRight, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_CameraRight), Rotate));

				DirectX::XMStoreFloat3(&m_CameraUp, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_CameraUp), Rotate));
			}

			if (0.f != XAxisRotation) {
				DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_CameraRight), DirectX::XMConvertToRadians(XAxisRotation));

				DirectX::XMStoreFloat3(&m_CameraLook, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_CameraLook), Rotate));

				DirectX::XMStoreFloat3(&m_CameraUp, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_CameraUp), Rotate));
			}

			DirectX::XMStoreFloat3(&m_CameraLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_CameraLook)));

			DirectX::XMStoreFloat3(&m_CameraRight, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&m_CameraUp), DirectX::XMLoadFloat3(&m_CameraLook))));

			DirectX::XMStoreFloat3(&m_CameraUp, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&m_CameraLook), DirectX::XMLoadFloat3(&m_CameraRight))));
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

	// �÷��̾��� �ִϸ��̼��� ������ �Ϸ�� ��� �ʱ�ȭ ����
	if (true == GetChangeState()) {
		if (m_State == STATE_RELOAD) m_CompletedReload = true;

		SetChangeState(false), m_State = STATE_NONE;
		m_CheckDamagedTime = 0.f;
		m_Camera->SetOffset(DirectX::XMFLOAT3(0.f, 25.f, -75.f));
	}

	if (m_Hp <= 0) m_State = STATE_DEATH;

	// Ű���� or ���콺 ��ɿ� ���� �÷��̾ ������ �ִϸ��̼��� ����
	switch (m_State) {
	case STATE_ROLL:
	{
		if (GetCurrentAnimationTrackIndex() != P_ROLL) SetAnimationTrack(P_ROLL, ANIMATION_TYPE_ONCE);

		DirectX::XMFLOAT3 TempPosition = GetPosition();

		float RollSpeed = m_ElapsedTime * 250.f;
		DirectX::XMFLOAT3 Position{};
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), RollSpeed)));
		m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;

		// ���� ���� ������ ����� ���� ��ġ�� ���ư�
		if (0 + 50.f >= m_TransformPos._41 || MAP_SIZE + 50.f <= m_TransformPos._41 || 0 + 50.f >= m_TransformPos._43 || MAP_SIZE + 50.f <= m_TransformPos._43)
			SetPosition(TempPosition);

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

		DirectX::XMFLOAT3 TempPosition = GetPosition();

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
		// ���� ���� ������ ����� ���� ��ġ�� ���ư�
		if (0 + 50.f >= m_TransformPos._41 || MAP_SIZE + 50.f <= m_TransformPos._41 || 0 + 50.f >= m_TransformPos._43 || MAP_SIZE + 50.f <= m_TransformPos._43)
			SetPosition(TempPosition);
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

	// �÷��̾��� Ű���� ��� �� �ൿ ���� (ex. ������ ���� �ǰ� or ��� ��)
	switch (m_State) {
	case STATE_DAMAGED:
	{
		if (P_DAMAGED != GetCurrentAnimationTrackIndex()) {
			// �ǰ� ȿ�� - �ǰ� �ִϸ��̼�, �Ͻ������� ī�޶��� �����°� �ؽ�ó ���� ����, ü�� ����
			SetAnimationTrack(P_DAMAGED, ANIMATION_TYPE_ONCE);
			m_Camera->SetOffset(DirectX::XMFLOAT3(0.f, 25.f, -30.f));
			if (0.f == m_CheckDamagedTime) {
				SetChangeTexcoords(DirectX::XMFLOAT4(1.f, -1.f, -1.f, -1.f));
				switch (m_HitMonsterKind) {
				case M_WEAKORC:
				{
					m_Hp -= 5;
				}
				break;

				case M_STRONGORC:
				case M_SHAMANORC:
				{
					m_Hp -= 10;
				}
				break;

				case M_WOLFRIDERORC:
				{
					m_Hp -= 20;
				}
				break;
				}
			}
		}
		if (m_CheckDamagedTime > 0.15f) SetChangeTexcoords(DirectX::XMFLOAT4(0.f, -1.f, -1.f, -1.f));

		m_CheckDamagedTime += m_ElapsedTime;
	}
	break;

	case STATE_DEATH:
	{
		if (P_DEATH != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_DEATH, ANIMATION_TYPE_ONCE);
	}
	break;
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