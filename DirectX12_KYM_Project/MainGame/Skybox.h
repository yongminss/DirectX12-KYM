#pragma once

#include "GameObject.h"

class Skybox : public GameObject
{
private:


public:
	Skybox();
	~Skybox();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void Render(ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 PlayerPosition);
};