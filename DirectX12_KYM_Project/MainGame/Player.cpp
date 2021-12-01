#include "stdafx.h"
#include "Player.h"

#include "Camera.h"


Player::Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	GameObject* Model = LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Monster_WeakOrc.bin", true);
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
	float MouseDistance = 0.f;

	if (GetCapture() == Hwnd) {
		GetCursorPos(&MousePos);
		MouseDistance = (MousePos.x - PreviousPos.x) / 10.f;
		SetCursorPos(PreviousPos.x, PreviousPos.y);

		if (MouseDistance != 0.f) SetRotate(DirectX::XMFLOAT3(0.f, MouseDistance, 0.f));
	}

	// Height Map의 높이에 따라 플레이어의 Y좌표를 변경
	float MapDistance = m_TransformPos._42 - MapY;

	if (abs(MapDistance) > 0.5f) {
		DirectX::XMFLOAT3 Position{};
		float Speed = -((MapDistance * 15.f) * m_ElapsedTime);
		DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetUp()), Speed)));
		SetPosition(Position);
	}

	// 입력이 호출되면 Player의 속도를 천천히 증가
	if (m_ActiveMove[0] == true || m_ActiveMove[1] == true || m_ActiveMove[2] == true || m_ActiveMove[3] == true) {
		if (m_Speed <= 150.f) m_Speed += m_ElapsedTime * 100.f;
	}

	// 1. 입력이 없으면 속도를 감소
	// 2. 상하 or 좌우 키가 동시에 입력되면 속도를 감소
	if ((m_ActiveMove[0] == false && m_ActiveMove[1] == false && m_ActiveMove[2] == false && m_ActiveMove[3] == false) ||
		(m_ActiveMove[0] == true && m_ActiveMove[1] == true) ||
		(m_ActiveMove[2] == true && m_ActiveMove[3] == true)) {
		m_Speed -= m_ElapsedTime * 500.f;
		// 속도가 감소하다가 0보다 작아지면 움직임 멈춤
		if (m_Speed <= 0.f) m_Speed = 0.f;
	}

	// m_Speed가 0이 아니면 움직이고 있는 상태이므로 Move 수행
	if (m_Speed != 0) {
		DirectX::XMFLOAT3 Position{};
		float Speed = m_Speed * m_ElapsedTime;

		// 속도에 따른 애니메이션 설정
		if (m_Speed >= 80.f) SetAnimationTrackIndex(2);
		else SetAnimationTrackIndex(1);

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
	}
	// m_Speed가 0이면 움직임이 없으므로 IDLE 상태로 설정
	else {
		// Walk or Run 애니메이션일 경우에만 IDEL로 변경
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