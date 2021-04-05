#pragma once

#include "Mesh.h"
#include "Shader.h"

// ���� ���忡 �����ϴ� ��� ������Ʈ
class GameObject
{
protected:
	Mesh *m_Mesh = nullptr;
	Shader *m_Shader = nullptr;

	DirectX::XMFLOAT4X4 m_WorldPos{};

public:
	GameObject();
	~GameObject();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void SetPosition(DirectX::XMFLOAT3 Position);
	DirectX::XMFLOAT3 GetPosition();
	void MoveForward();
	void MoveBackward();

	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Render(ID3D12GraphicsCommandList* CommandList);
};