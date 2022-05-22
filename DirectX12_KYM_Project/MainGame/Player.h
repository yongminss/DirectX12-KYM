#pragma once

#include "GameObject.h"

#define STATE_NONE 0
#define STATE_SHOOT 1
#define STATE_RELOAD 2
#define STATE_ROLL 3

class Camera;

// 카메라를 가지고 있고 플레이어가 조종할 수 있는 오브젝트
class Player : public GameObject
{
private:
	Camera *m_Camera = nullptr;

	float m_Speed = 0.f;
	bool m_ActiveMove[4]{};

	int m_State = STATE_NONE;

	float m_RollDistance = 0.f;

	DirectX::XMFLOAT4X4 m_PreviousCameraTransformPos{};
	float m_Pitch = 0.f;

	float m_ElapsedTime = 0.f;

public:
	Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~Player();

	void ActiveMove(int Index, bool Active) { m_ActiveMove[Index] = Active; }

	void ActiveShoot() { m_State = STATE_SHOOT; }
	void ActiveReload() { m_State = STATE_RELOAD; }
	void ActiveRoll() { m_State = STATE_ROLL; }

	DirectX::XMFLOAT3 GetCameraRight() { return DirectX::XMFLOAT3(m_PreviousCameraTransformPos._11, m_PreviousCameraTransformPos._12, m_PreviousCameraTransformPos._13); }
	DirectX::XMFLOAT3 GetCameraUp() { return DirectX::XMFLOAT3(m_PreviousCameraTransformPos._21, m_PreviousCameraTransformPos._22, m_PreviousCameraTransformPos._23); }
	DirectX::XMFLOAT3 GetCameraLook() { return DirectX::XMFLOAT3(m_PreviousCameraTransformPos._31, m_PreviousCameraTransformPos._32, m_PreviousCameraTransformPos._33); }
	DirectX::XMFLOAT3 GetCameraPosition() { return DirectX::XMFLOAT3(m_PreviousCameraTransformPos._41, m_PreviousCameraTransformPos._42, m_PreviousCameraTransformPos._43); }

	DirectX::XMFLOAT3 GetCameraWorldRight();
	DirectX::XMFLOAT3 GetCameraWorldUp();
	DirectX::XMFLOAT3 GetCameraWorldLook();
	DirectX::XMFLOAT3 GetCameraWorldPosition();

	void Move(HWND Hwnd, POINT PreviousPos, float MapY);

	void Animate(float ElapsedTime, HWND Hwnd, POINT PreviousPos, float MapY);
	void UpdateCamera(ID3D12GraphicsCommandList* CommandList);
};