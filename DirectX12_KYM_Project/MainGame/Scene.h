#pragma once

#include "Shader.h"

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
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