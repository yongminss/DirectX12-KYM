#pragma once
// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* &Device);
};