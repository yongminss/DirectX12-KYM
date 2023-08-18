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
	bool m_GetHpItem = false;
	float m_Speed = 0.f;
	float m_RollDistance = 0.f;
	int m_HitMonsterKind = 0;
	float m_CheckDamagedTime = 0.f;
	float m_BurnTime = 0.f;

	int m_State = STATE_NONE;

	float m_Pitch = 0.f;
	float m_Yaw = 0.f;

	int m_Hp = 100;
	int m_Power = 5;

	DirectX::XMFLOAT3 m_CameraRight = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
	DirectX::XMFLOAT3 m_CameraUp = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
	DirectX::XMFLOAT3 m_CameraLook = DirectX::XMFLOAT3(0.f, 0.f, 1.f);

public:
	Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~Player();

	void ActiveMove(int Index, bool Active) { m_ActiveMove[Index] = Active; }

	void ActiveShoot() { m_State = STATE_SHOOT; }
	void ActiveReload() { m_State = STATE_RELOAD; }
	void ActiveRoll() { m_State = STATE_ROLL; }
	void ActiveDamaged(int MonsterKind) { m_State = STATE_DAMAGED, m_HitMonsterKind = MonsterKind; }

	void SetCompletedReload(bool CompletedReload) { m_CompletedReload = CompletedReload; }
	void SetPower(int Power) { m_Power = Power; }

	void SetOffset(DirectX::XMFLOAT3 Offset);

	Camera* GetCamera() { return m_Camera; }

	bool GetCompletedReload() { return m_CompletedReload; }
	int GetHp() { return m_Hp; }
	int GetPower() { return m_Power; }
	
	DirectX::XMFLOAT3 GetCameraRight() { return m_CameraRight; }
	DirectX::XMFLOAT3 GetCameraUp() { return m_CameraUp; }
	DirectX::XMFLOAT3 GetCameraLook() { return m_CameraLook; }

	void ActiveGetHpItem() { m_GetHpItem = true; }

	void RotateByMessage(HWND Hwnd, POINT PreviousPos);
	void Move(float ElapsedTime, float MapY);

	void Animate(float ElapsedTime, HWND Hwnd, POINT PreviousPos, float MapY);
	void UpdateCamera(ID3D12GraphicsCommandList* CommandList);
};