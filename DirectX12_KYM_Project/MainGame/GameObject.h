#pragma once

class Mesh;
class Shader;
class Material;
class AnimationController;

// 게임 월드에 등장하는 모든 오브젝트
class GameObject
{
protected:
	Mesh *m_Mesh = nullptr;
	Material *m_Material = nullptr;
	AnimationController *m_AnimationController = nullptr;

	DirectX::XMFLOAT4X4 m_WorldPos{};
	DirectX::XMFLOAT4X4 m_TransformPos{};

	char m_FrameName[64]{};

	GameObject *m_Sibling = nullptr;
	GameObject *m_Child = nullptr;

	int m_ShaderType = 0;

public:
	GameObject();
	GameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature);
	~GameObject();

	static GameObject* LoadBinaryFileModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, const char* FileName, Shader* InstanceShader, bool ActiveAnimation);
	static GameObject* LoadFrameHierarchy(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File, Shader* InstanceShader);
	void LoadAnimationInfo(FILE* File);

	void SetMesh(Mesh* UsingMesh);
	void SetMaterial(Material* UsingMaterial);

	void SetRight(DirectX::XMFLOAT3 Right);
	void SetUp(DirectX::XMFLOAT3 Up);
	void SetLook(DirectX::XMFLOAT3 Look);
	void SetPosition(DirectX::XMFLOAT3 Position);
	void SetTransformPos(DirectX::XMFLOAT4X4 TransformPos);

	void SetScale(DirectX::XMFLOAT3 Size);
	void SetRotate(DirectX::XMFLOAT3 Angle);

	void SetFrameName(char* FrameName) { memcpy(m_FrameName, FrameName, sizeof(m_FrameName)); }

	void SetChild(GameObject* Child);

	void SetMeshBoneFrame(GameObject* RootFrame);

	void SetAnimationTrack(int Index, int Type);

	DirectX::XMFLOAT3 GetRight() { return DirectX::XMFLOAT3(m_TransformPos._11, m_TransformPos._12, m_TransformPos._13); }
	DirectX::XMFLOAT3 GetUp() { return DirectX::XMFLOAT3(m_TransformPos._21, m_TransformPos._22, m_TransformPos._23); }
	DirectX::XMFLOAT3 GetLook() { return DirectX::XMFLOAT3(m_TransformPos._31, m_TransformPos._32, m_TransformPos._33); }
	DirectX::XMFLOAT3 GetPosition() { return DirectX::XMFLOAT3(m_TransformPos._41, m_TransformPos._42, m_TransformPos._43); }
	DirectX::XMFLOAT4X4 GetWorldPos() { return m_WorldPos; }
	char* GetFrameName() { return m_FrameName; }

	float GetCollisionMeshDistance();
	int GetCurrentAnimationTrackIndex();

	GameObject* FindFrame(char* FrameName);

	GameObject* CheckCollision(DirectX::XMFLOAT3 StartPos, DirectX::XMFLOAT3 EndPos);

	void UpdateTransform(DirectX::XMFLOAT4X4* Parents);

	virtual void Animate(float ElapsedTime);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
	virtual void Render(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView, int ModelCount);
};