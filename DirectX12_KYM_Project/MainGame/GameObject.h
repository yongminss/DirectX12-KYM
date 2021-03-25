#pragma once

#include "Mesh.h"
#include "Shader.h"

// 게임 월드에 등장하는 모든 오브젝트
class GameObject
{
private:
	Mesh *m_Mesh = nullptr;
	Shader *m_Shader = nullptr;

	DirectX::XMFLOAT4X4 m_WorldPos{};

public:
	GameObject();
	~GameObject();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);

	void Render(ID3D12GraphicsCommandList* CommandList);
};