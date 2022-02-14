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

	// Player 오브젝트가 바라보는 화면을 플레이어에게 보여줄 수 있게 Camera 생성
	m_Camera = new Camera();
	m_Camera->CreateCamera(Device, CommandList);

	// 플레이어가 크기를 축소했으므로 카메라 좌표도 축소
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
	// 마우스 입력으로 플레이어의 방향을 전환
	POINT MousePos{};
	float YAxisRotation = 0.f, XAxisRotation = 0.f;

	if (GetCapture() == Hwnd) {
		GetCursorPos(&MousePos);
		YAxisRotation = (MousePos.x - PreviousPos.x) / 10.f;
		XAxisRotation = (MousePos.y - PreviousPos.y) / 10.f;
		SetCursorPos(PreviousPos.x, PreviousPos.y);

		// 마우스 입력이 있으면 플레이어와 카메라를 회전
		if (YAxisRotation != 0.f || XAxisRotation != 0.f) {
			// 카메라의 X축 회전을 위해 누적된 회전량을 저장 - 플레이어는 X축 회전을 하지 않음
			m_Pitch += XAxisRotation;

			// 카메라의 뒤집힘 방지를 위해 최대 Pitch 값을 설정
			if (m_Pitch >= 80.f || m_Pitch <= -50.f) {
				m_Pitch -= XAxisRotation;
			}
			// X축 회전이 최대에 도달하지 않았으면 회전을 수행
			else {
				// 마우스에 입력된 회전량을 바탕으로 X or Y축 회전을 수행
				if ((XAxisRotation > -0.2f || XAxisRotation < 0.2f) && (YAxisRotation > 0.5f || YAxisRotation < -0.5f)) {
					// 마우스의 위아래 움직임이 적고 좌우의 움직임이 크다면 플레이어를 Y축 회전
					SetRotate(DirectX::XMFLOAT3(0.f, YAxisRotation, 0.f));
					m_PreviousCameraTransformPos = m_TransformPos;
					m_Pitch = 0.f;
				}
				else {
					// 플레이어는 그대로, 카메라만 X축 회전
					DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(XAxisRotation), DirectX::XMConvertToRadians(0.f), DirectX::XMConvertToRadians(0.f));
					DirectX::XMStoreFloat4x4(&m_PreviousCameraTransformPos, DirectX::XMMatrixMultiply(Rotate, DirectX::XMLoadFloat4x4(&m_PreviousCameraTransformPos)));
				}
			}
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
	if (m_Speed != 0) {
		DirectX::XMFLOAT3 Position{};
		float Speed = m_Speed * m_ElapsedTime;

		// 움직이고 있는 상태이므로 애니메이션도 Run으로 설정
		if (GetCurrentAnimationTrackIndex() != P_ROLL) SetAnimationTrack(P_RUN, ANIMATION_TYPE_LOOP);

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
		// 회전 여부와 관계 없이 카메라의 월드 좌표는 이동해야하므로 플레이어의 월드 좌표를 대입
		m_PreviousCameraTransformPos._41 = m_TransformPos._41, m_PreviousCameraTransformPos._42 = m_TransformPos._42, m_PreviousCameraTransformPos._43 = m_TransformPos._43;
	}
	// m_Speed가 0이면 움직임이 없으므로 IDLE 상태로 설정
	else {
		// Walk or Run 애니메이션일 경우에만 IDLE로 변경
		if (GetCurrentAnimationTrackIndex() == P_RUN) SetAnimationTrack(P_IDLE, ANIMATION_TYPE_LOOP);
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