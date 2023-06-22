#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Player_Soldier.bin", nullptr, true);
	SetChild(Model);

	// Player 오브젝트가 바라보는 화면을 플레이어에게 보여줄 수 있게 Camera 생성
	m_Camera = new Camera();
	m_Camera->CreateCamera(Device, CommandList);
}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

void Player::Move(HWND Hwnd, POINT PreviousPos, float MapY)
{
	// 마우스 입력으로 플레이어의 방향을 전환
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

		// 마우스 입력이 있으면 플레이어와 카메라의 회전
		if (0.f != YAxisRotation || 0.f != XAxisRotation) {
			// 마우스가 x축으로 이동하면 플레이어의 y축을 회전
			if (0.f != YAxisRotation) {
				DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMConvertToRadians(YAxisRotation));

				DirectX::XMFLOAT3 Look{};
				DirectX::XMStoreFloat3(&Look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&GetLook()), Rotate));
				SetLook(Look);

				DirectX::XMFLOAT3 Right{};
				DirectX::XMStoreFloat3(&Right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&GetRight()), Rotate));
				SetRight(Right);
			}

			// 플레이어의 회전이 수행됐으므로 카메라의 회전 수행
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
	// Height Map의 높이에 따라 플레이어의 Y좌표를 변경
	float MapDistance = (m_TransformPos._42 - 15.f) - MapY;

	if (abs(MapDistance) > 0.05f) {
		DirectX::XMFLOAT3 Position{};
		float Speed = -((MapDistance * 15.f) * m_ElapsedTime);
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetUp()), Speed)));
		SetPosition(Position);
	}

	// 플레이어의 애니메이션이 수행이 완료된 경우 초기화 수행
	if (true == GetChangeState()) {
		if (m_State == STATE_RELOAD) m_CompletedReload = true;

		SetChangeState(false), m_State = STATE_NONE;
		m_CheckDamagedTime = 0.f;
		m_Camera->SetOffset(DirectX::XMFLOAT3(0.f, 25.f, -75.f));
	}

	if (m_Hp <= 0) m_State = STATE_DEATH;

	// 키보드 or 마우스 명령에 따라 플레이어가 수행할 애니메이션을 결정
	switch (m_State) {
	case STATE_ROLL:
	{
		if (GetCurrentAnimationTrackIndex() != P_ROLL) SetAnimationTrack(P_ROLL, ANIMATION_TYPE_ONCE);

		DirectX::XMFLOAT3 TempPosition = GetPosition();

		float RollSpeed = m_ElapsedTime * 250.f;
		DirectX::XMFLOAT3 Position{};
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), RollSpeed)));
		m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y, m_TransformPos._43 = Position.z;

		// 맵의 범위 밖으로 벗어나면 이전 위치로 돌아감
		if (0 + 50.f >= m_TransformPos._41 || MAP_SIZE + 50.f <= m_TransformPos._41 || 0 + 50.f >= m_TransformPos._43 || MAP_SIZE + 50.f <= m_TransformPos._43)
			SetPosition(TempPosition);

		m_RollDistance += RollSpeed;

		// 일정 거리를 이동했으면 구르기 종료
		if (m_RollDistance >= 200.f) m_State = STATE_NONE, m_RollDistance = 0.f;
	}
	break;
	}

	// 입력이 호출되면 Player의 속도를 천천히 증가
	if (m_ActiveMove[0] == true || m_ActiveMove[1] == true || m_ActiveMove[2] == true || m_ActiveMove[3] == true) {
		if (m_Speed <= 150.f) m_Speed += m_ElapsedTime * 500.f;
	}

	// 1. 입력이 없으면 속도를 감소
	// 2. 상하 or 좌우 키가 동시에 입력되면 속도를 감소
	if ((m_ActiveMove[0] == false && m_ActiveMove[1] == false && m_ActiveMove[2] == false && m_ActiveMove[3] == false) ||
		(m_ActiveMove[0] == true && m_ActiveMove[1] == true) ||
		(m_ActiveMove[2] == true && m_ActiveMove[3] == true)) {
		m_Speed -= m_ElapsedTime * 1000.f;
		// 속도가 감소하다가 0보다 작아지면 움직임 멈춤
		if (m_Speed <= 0.f) m_Speed = 0.f;
	}

	// m_Speed가 0이 아니면 움직이고 있는 상태이므로 Move 수행
	if (m_Speed != 0 && STATE_ROLL != m_State) {
		DirectX::XMFLOAT3 Position{};
		float Speed = m_Speed * m_ElapsedTime;

		switch (m_State) {
		case STATE_NONE:
		{
			// 전방 이동
			if (true == m_ActiveMove[0]) {
				SetAnimationTrack(P_RUN_FRONT, ANIMATION_TYPE_LOOP);
				// 전방 + 좌
				if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_FRONT_LEFT, ANIMATION_TYPE_LOOP);
				// 전방 + 우
				if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_FRONT_RIGHT, ANIMATION_TYPE_LOOP);
			}
			// 후방 이동
			else if (true == m_ActiveMove[1]) {
				SetAnimationTrack(P_RUN_BACK, ANIMATION_TYPE_LOOP);
				// 후방 + 좌
				if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_BACK_LEFT, ANIMATION_TYPE_LOOP);
				// 후방 + 우
				if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_BACK_RIGHT, ANIMATION_TYPE_LOOP);
			}
			// 좌측 이동
			else if (true == m_ActiveMove[2]) SetAnimationTrack(P_RUN_LEFT, ANIMATION_TYPE_LOOP);
			// 우측 이동
			else if (true == m_ActiveMove[3]) SetAnimationTrack(P_RUN_RIGHT, ANIMATION_TYPE_LOOP);
		}
		break;

		case STATE_SHOOT: // 움직이는 상태에서 공격 애니메이션을 수행
		{
			// 1. Shoot or Reload Animation은 게임 내에서 상호작용 발생 - 반복이 아닌 한 번의 수행만 해야 함 (Type - Convert)
			// 2. 이전 프레임에서 10번째 Idle_Shoot이 진행되던 중에 플레이어 이동이 발생하면 11번째 Run_Shoot으로 애니메이션을 이어서 수행하게 설정
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			// 이전 프레임의 애니메이션과 새로 설정될 애니메이션의 번호를 비교하여 전환 여부를 결정
			if (P_SHOOT <= NowAnimationIndex && P_RELOAD > NowAnimationIndex) AnimateConversion = true;

			// 전방 이동
			if (m_ActiveMove[0]) {
				// 전방 + 좌
				if (m_ActiveMove[2]) {
					if (P_RUN_SHOOT_FRONT_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// 전방 + 우
				else if (m_ActiveMove[3]) {
					if (P_RUN_SHOOT_FRONT_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// 전방
				else {
					if (P_RUN_SHOOT_FRONT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_FRONT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// 후방 이동
			else if (m_ActiveMove[1]) {
				// 후방 + 좌
				if (m_ActiveMove[2]) {
					if (P_RUN_SHOOT_BACK_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// 후방 + 우
				else if (m_ActiveMove[3]) {
					if (P_RUN_SHOOT_BACK_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
				// 후방
				else {
					if (P_RUN_SHOOT_BACK != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_BACK, ANIMATION_TYPE_CONVERT, AnimateConversion);
				}
			}
			// 좌측 이동
			else if (m_ActiveMove[2]) {
				if (P_RUN_SHOOT_LEFT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
			// 우측 이동
			else if (m_ActiveMove[3]) {
				if (P_RUN_SHOOT_RIGHT != NowAnimationIndex) SetAnimationTrack(P_RUN_SHOOT_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
		}
		break;

		case STATE_RELOAD: // Shoot과 마찬가지로 전환에 대한 처리를 수행
		{
			bool AnimateConversion = false;
			int NowAnimationIndex = GetCurrentAnimationTrackIndex();

			if (P_RELOAD <= NowAnimationIndex && P_ROLL > NowAnimationIndex) AnimateConversion = true;

			// 앞
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
			// 뒤
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
			// 좌
			else if (true == m_ActiveMove[2]) {
				if (P_RUN_RELOAD_LEFT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_LEFT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
			// 우
			else if (true == m_ActiveMove[3]) {
				if (P_RUN_RELOAD_RIGHT != GetCurrentAnimationTrackIndex()) SetAnimationTrack(P_RUN_RELOAD_RIGHT, ANIMATION_TYPE_CONVERT, AnimateConversion);
			}
		}
		break;
		}

		DirectX::XMFLOAT3 TempPosition = GetPosition();

		// Set Position - ActiveMove : 0(앞), 1(뒤), 2(좌), 3(우)
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
		// 맵의 범위 밖으로 벗어나면 이전 위치로 돌아감
		if (0 + 50.f >= m_TransformPos._41 || MAP_SIZE + 50.f <= m_TransformPos._41 || 0 + 50.f >= m_TransformPos._43 || MAP_SIZE + 50.f <= m_TransformPos._43)
			SetPosition(TempPosition);
	}
	// 움직이지 않는 상태일 때의 애니메이션을 결정
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

	// 플레이어의 키보드 명령 외 행동 수행 (ex. 몬스터의 공격 피격 or 사망 등)
	switch (m_State) {
	case STATE_DAMAGED:
	{
		if (P_DAMAGED != GetCurrentAnimationTrackIndex()) {
			// 피격 효과 - 피격 애니메이션, 일시적으로 카메라의 오프셋과 텍스처 색상 변경, 체력 감소
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