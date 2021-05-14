#pragma once

#include "GameObject.h"
#include "Player.h"
#include "UserInterface.h"
#include "Terrain.h"

// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
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