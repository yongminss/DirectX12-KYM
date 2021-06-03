#pragma once

#include "GameObject.h"
#include "Camera.h"

// 카메라를 가지고 있고 플레이어가 조종할 수 있는 오브젝트
class Player : public GameObject
{
private:
	Camera *m_Camera = nullptr;

	bool m_ActiveMove[4]{};

public:
	Player();
	~Player();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void ActiveMove(int Index, bool Active) { m_ActiveMove[Index] = Active; }

	void Move(int Index, float Distance);

	void Animate(float ElapsedTime);
	void Render(ID3D12GraphicsCommandList* CommandList);
};