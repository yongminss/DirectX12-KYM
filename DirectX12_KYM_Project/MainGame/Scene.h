#pragma once

#include "Shader.h"

// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	Shader *m_Shader = nullptr;

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateShader(ID3D12Device* Device);

	void Render(ID3D12GraphicsCommandList* CommandList);
};