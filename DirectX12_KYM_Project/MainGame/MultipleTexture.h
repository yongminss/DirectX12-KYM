#pragma once

#include "GameObject.h"

class MultipleTexture : public GameObject
{
private:
	int m_TextureCount = 0;
	bool m_ActiveAnimate = false;
	float m_AnimateTime = 0.f;

	DirectX::XMFLOAT3 m_TargetPosition{};

public:
	MultipleTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);
	~MultipleTexture();

	void ActiveAnimate() { m_ActiveAnimate = true; }

	float GetAnimateTime() { return m_AnimateTime; }

	void Animate(float ElapsedTime, DirectX::XMFLOAT3 Position);
	void Render(ID3D12GraphicsCommandList* CommandList);
};