#pragma once

#include "GameObject.h"

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	D3D12_VIEWPORT m_Viewport = { 0, 0, 800, 600, 0.f, 1.f };
	D3D12_RECT m_ScissorRect = { 0, 0, 800, 600 };

	GameObject *m_GameObject = nullptr;

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void Render(ID3D12GraphicsCommandList* CommandList);
};