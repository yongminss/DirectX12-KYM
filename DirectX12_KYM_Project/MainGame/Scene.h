#pragma once
// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* &Device);
};