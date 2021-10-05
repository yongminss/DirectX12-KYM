#pragma once

#include "GameObject.h"

class Terrain : public GameObject
{
private:
	BYTE *m_HeightMapPos = nullptr;

public:
	Terrain(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~Terrain();

	void LoadHeightMapFile(int Width, int Length);

	float GetHeightMapYPos(int x, int z);
};