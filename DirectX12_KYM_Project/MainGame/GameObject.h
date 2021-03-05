#pragma once

#include "Mesh.h"
#include "Shader.h"

class GameObject
{
private:
	Mesh *m_Mesh = nullptr;
	Shader *m_Shader = nullptr;

public:
	GameObject();
	~GameObject();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void Render(ID3D12GraphicsCommandList* CommandList);
};