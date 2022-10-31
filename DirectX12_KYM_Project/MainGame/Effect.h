#pragma once

#include "GameObject.h"

// Scene에서 사용하는 효과 (ex. 플레이어의 총알 발사, 몬스터의 플레이어 인식 등)
class Effect : public GameObject
{
private:
	bool m_Active = false;
	float m_AnimateTime = 0.f;

	DirectX::XMFLOAT4X4 m_TargetTransformPos{};

public:
	Effect(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);
	~Effect();

	void ActiveEffect(DirectX::XMFLOAT4X4 TargetPos) { m_Active = true, m_AnimateTime = 0.f, m_TargetTransformPos = TargetPos; }

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};