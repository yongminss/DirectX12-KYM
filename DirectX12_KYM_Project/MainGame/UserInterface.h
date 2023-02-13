#pragma once

#include "GameObject.h"

class UserInterface : public GameObject
{
protected:
	bool m_Active = true;
	float m_AnimateTime = 0.f;

public:
	UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);
	~UserInterface();

	void SetActive(bool Active) { m_Active = Active; }

	virtual void Animate(float ElapsedTime, int Hp);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};