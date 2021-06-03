#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

// 게임 월드에 등장하는 모든 오브젝트
class GameObject
{
protected:
	Mesh **m_Mesh = nullptr;
	Shader *m_Shader = nullptr;
	Texture *m_Texture = nullptr;

	int m_MeshCount = 0;

	DirectX::XMFLOAT4X4 m_WorldPos{};

public:
	GameObject();
	~GameObject();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	void SetMesh(int MeshIndex, Mesh* ObjectMesh);
	void SetShader(Shader* ObjectShader);
	void SetTexture(Texture* ObjectTexture);

	void SetRight(DirectX::XMFLOAT3 Right);
	void SetUp(DirectX::XMFLOAT3 Up);
	void SetLook(DirectX::XMFLOAT3 Look);
	void SetPosition(DirectX::XMFLOAT3 Position);

	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetLook();
	DirectX::XMFLOAT3 GetPosition();

	void Rotate(DirectX::XMFLOAT3 Angle);

	void Animate(float ElapsedTime);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Render(ID3D12GraphicsCommandList* CommandList);
};