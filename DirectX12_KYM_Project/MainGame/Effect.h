#pragma once

#include "GameObject.h"

// Scene에서 사용하는 효과 (ex. 플레이어의 총알 발사, 몬스터의 플레이어 인식 등)
class Effect : public GameObject
{
private:
	bool m_ActiveAnimate = false;
	bool m_ActiveRender = false;
	bool m_Collision = false;
	bool m_FlameOffSignal = false;
	float m_AnimateTime = 0.f;
	float m_CheckTime = 0.f;
	float m_PreviousYPos = 50.f;
	int m_StackDecrease = 0;
	DirectX::XMFLOAT4X4 m_TargetTransformPos{};

public:
	Effect(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind, int Index = 0);
	~Effect();

	void ActiveEffect(DirectX::XMFLOAT4X4 TargetPos) { m_ActiveAnimate = true, m_TargetTransformPos = TargetPos; }
	void ActiveCollision() { m_Collision = true; }
	void FalseFlameOffSignal() { m_FlameOffSignal = false, m_TransformPos._41 = 1000.f; }
	void SetTargetTransformPos(DirectX::XMFLOAT4X4 TargetPos) { m_TargetTransformPos = TargetPos; }

	bool GetCollision() { return m_Collision; }
	bool GetFlameOffSignal() { return m_FlameOffSignal; }
	int GetStackDecrease() { return m_StackDecrease; }

	virtual void Animate(float ElapsedTime, int Index = 0);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};