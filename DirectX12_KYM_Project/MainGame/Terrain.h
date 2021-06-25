#pragma once

#include "GameObject.h"

class Terrain : public GameObject
{
private:
	BYTE *m_HeightMapPos = nullptr;

public:
	Terrain();
	~Terrain();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	float GetHeightMapYPos(int x, int z) { return m_Mesh[0]->GetHeightMapYPos(x, z); }

	void LoadHeightMapFile(int Width, int Length);
};