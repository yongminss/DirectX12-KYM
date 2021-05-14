#pragma once

#include "GameObject.h"
#include "Player.h"
#include "UserInterface.h"
#include "Terrain.h"

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	Player *m_Player = nullptr;
	UserInterface *m_UserInterface = nullptr;
	Terrain *m_Terrain = nullptr;
	std::vector<GameObject*> m_GameObjects{};

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
};