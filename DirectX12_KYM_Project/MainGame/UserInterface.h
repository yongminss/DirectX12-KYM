#pragma once

#include "GameObject.h"

class UserInterface : public GameObject
{
private:

public:
	UserInterface();
	~UserInterface();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
};