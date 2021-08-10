#pragma once

#include "GameObject.h"

class Skybox : public GameObject
{
private:
	Mesh **m_SkyboxMesh = nullptr;

public:
	Skybox();
	~Skybox();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void SetMesh(int Index, Mesh* UsingMesh) { m_SkyboxMesh[Index] = UsingMesh; }

	void Animate(float ElapsedTime, DirectX::XMFLOAT3 Position);
	void Render(ID3D12GraphicsCommandList* CommandList);
};