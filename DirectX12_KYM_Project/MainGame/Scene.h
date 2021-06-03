#pragma once

#include "GameObject.h"
#include "Player.h"
#include "UserInterface.h"
#include "Terrain.h"
#include "Skybox.h"

// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	Player *m_Player = nullptr;
	Terrain *m_Terrain = nullptr;
	Skybox *m_Skybox = nullptr;
	UserInterface *m_UserInterface = nullptr;
	std::vector<GameObject*> m_GameObjects{};

	POINT m_PreviousPos{};

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void Animate(float ElapsedTime);
	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam);
};