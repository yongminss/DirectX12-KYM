#pragma once

#include "Mesh.h"
#include "Material.h"

// 게임 월드에 등장하는 모든 오브젝트
class GameObject
{
protected:
	Mesh *m_Mesh = nullptr;
	Material *m_Material = nullptr;

	DirectX::XMFLOAT4X4 m_WorldPos{};
	DirectX::XMFLOAT4X4 m_TransformPos{};

	char m_FrameName[64]{};

public:
	GameObject();
	~GameObject();

	void CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);

	GameObject* LoadBinaryFileModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, const char* FileName);
	GameObject* LoadFrameHierarchy(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File);

	void SetMesh(Mesh* UsingMesh) { m_Mesh = UsingMesh; }
	void SetMaterial(Material* UsingMaterial) { m_Material = UsingMaterial; }

	void SetRight(DirectX::XMFLOAT3 Right);
	void SetUp(DirectX::XMFLOAT3 Up);
	void SetLook(DirectX::XMFLOAT3 Look);
	void SetPosition(DirectX::XMFLOAT3 Position);

	void SetTransformPos(DirectX::XMFLOAT4X4 TransformPos);

	void SetFrameName(char* FrameName) { memcpy(m_FrameName, FrameName, sizeof(m_FrameName)); }

	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetLook();
	DirectX::XMFLOAT3 GetPosition();

	char* GetFrameName() { return m_FrameName; }

	void Rotate(DirectX::XMFLOAT3 Angle);

	void Animate(float ElapsedTime);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Render(ID3D12GraphicsCommandList* CommandList);
};