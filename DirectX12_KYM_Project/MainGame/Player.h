#pragma once

#include "GameObject.h"
#include "Camera.h"

// ī�޶� ������ �ְ� �÷��̾ ������ �� �ִ� ������Ʈ
class Player : public GameObject
{
private:
	Camera *m_Camera = nullptr;

public:
	Player();
	~Player();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void Render(ID3D12GraphicsCommandList* CommandList);
};