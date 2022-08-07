#pragma once

#include "GameObject.h"

class UserInterface : public GameObject
{
protected:
	int m_Kind = -1;

public:
	UserInterface(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);
	~UserInterface();

	virtual void Animate(float ElapsedTime, int Hp);
};