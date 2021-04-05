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

	void CreatePlayer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};