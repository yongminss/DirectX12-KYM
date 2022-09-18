#pragma once

#include "GameObject.h"

class UserInterface : public GameObject
{
protected:
	int m_Kind = -1;
	bool m_Active = true;

public:
	UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind, int NumbersIndex = 0);
	~UserInterface();

	virtual void Animate(float ElapsedTime, int Hp);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};