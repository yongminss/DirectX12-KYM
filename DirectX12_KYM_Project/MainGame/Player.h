#pragma once

#include "GameObject.h"

class Camera;

// ī�޶� ������ �ְ� �÷��̾ ������ �� �ִ� ������Ʈ
class Player : public GameObject
{
private:
	Camera *m_Camera = nullptr;

	bool m_ActiveMove[4]{};

	float m_ElapsedTime = 0.f;

public:
	Player(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~Player();

	void ActiveMove(int Index, bool Active) { m_ActiveMove[Index] = Active; }

	void Move(int Index, float Distance);

	void Animate(float ElapsedTime);
	void Render(ID3D12GraphicsCommandList* CommandList);
};