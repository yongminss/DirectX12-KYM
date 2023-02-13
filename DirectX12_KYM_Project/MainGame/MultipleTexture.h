#pragma once

#include "GameObject.h"

class MultipleTexture : public GameObject
{
private:
	int m_TextureCount = 0;

	DirectX::XMFLOAT3 m_TargetPosition{};

public:
	MultipleTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);
	~MultipleTexture();

	void Animate(float ElapsedTime, DirectX::XMFLOAT3 Position);
	void Render(ID3D12GraphicsCommandList* CommandList);
};