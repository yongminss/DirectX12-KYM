#pragma once

#include "GameObject.h"

// Scene���� ����ϴ� ȿ�� (ex. �÷��̾��� �Ѿ� �߻�, ������ �÷��̾� �ν� ��)
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