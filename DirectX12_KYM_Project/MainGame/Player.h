#pragma once

#include "GameObject.h"

#define STATE_NONE 0
#define STATE_SHOOT 1
#define STATE_RELOAD 2
#define STATE_ROLL 3
#define STATE_DAMAGED 4
#define STATE_DEATH 5

class Camera;

// 카메라를 가지고 있고 플레이어가 조종할 수 있는 오브젝트
class Player : public GameObject
{
private:
	Camera *m_Camera = nullptr;

	bool m_ActiveMove[4]{};
	bool m_CompletedReload = false;
	float m_Speed = 0.f;
	float m_RollDistance = 0.f;
	float m_CheckDamagedTime = 0.f;

	int m_State = STATE_NONE;

	float m_Pitch = 0.f;

	int m_Hp = 100;

	DirectX::XMFLOAT3 m_CameraRight = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
	DirectX::XMFLOAT3 m_CameraUp = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
	DirectX::XMFLOAT3 m_CameraLook = DirectX::XMFLOAT3(0.f, 0.f, 1.f);
	DirectX::XMFLOAT3 m_CameraPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);

	float m_ElapsedTime = 0.f;

public:
	Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~Player();

	void ActiveMove(int Index, bool Active) { m_ActiveMove[Index] = Active; }

	void ActiveShoot() { m_State = STATE_SHOOT; }
	void ActiveReload() { m_State = STATE_RELOAD; }
	void ActiveRoll() { m_State = STATE_ROLL; }
	void ActiveDamaged() { m_State = STATE_DAMAGED; }

	void SetCompletedReload(bool CompletedReload) { m_CompletedReload = CompletedReload; }

	Camera* GetCamera() { return m_Camera; }

	bool GetCompletedReload() { return m_CompletedReload; }
	int GetHp() { return m_Hp; }
	
	DirectX::XMFLOAT3 GetCameraRight() { return m_CameraRight; }
	DirectX::XMFLOAT3 GetCameraUp() { return m_CameraUp; }
	DirectX::XMFLOAT3 GetCameraLook() { return m_CameraLook; }
	DirectX::XMFLOAT3 GetCameraPosition() { return m_CameraPosition; }

	void Move(HWND Hwnd, POINT PreviousPos, float MapY);

	void Animate(float ElapsedTime, HWND Hwnd, POINT PreviousPos, float MapY);
	void UpdateCamera(ID3D12GraphicsCommandList* CommandList);
};