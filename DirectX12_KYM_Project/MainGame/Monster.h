#pragma once

#include "GameObject.h"

class Terrain;
class Effect;

class Monster : public GameObject
{
private:
	bool m_ActiveMonster = false;

	int m_Type = 0; // 몬스터 종류 (ex. Weak or Strong or Shaman or WolfRider)
	int m_State = 0;

	bool m_FirstHit = false;
	float m_HitTime = 0.f;

	bool m_ActiveDamaged = false;
	bool m_Death = false;
	
	float m_AnimateTime = 0.f;

	int m_Hp = 100;

public:
	Monster(int Type);
	~Monster();

	void SetState(int State) { m_State = State; }
	void ActiveDamaged() { m_ActiveDamaged = true; }

	void MoveToPlayer(float ElapsedTime, DirectX::XMFLOAT3 TargetPosition, Effect* Signal);
	void Animate(float ElapsedTime, DirectX::XMFLOAT3 TargetPosition, Terrain* GetTerrain, Effect* Signal);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};