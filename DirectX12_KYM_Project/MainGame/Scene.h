#pragma once

#include "GameObject.h"
#include "Player.h"

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	Player *m_Player = nullptr;
	std::vector<GameObject*> m_GameObjects{};

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM wParam);
};