#pragma once

class GameObject;

// ������Ʈ�� ����� Vertex Buffer ���� �� ����
class Mesh
{
protected:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	ID3D12Resource *m_IndexBuffer = nullptr;
	ID3D12Resource *m_UploadIndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView{};

	unsigned int m_VertexCount = 0;
	unsigned int m_IndexCount = 0;

	DirectX::BoundingBox m_BoundingBox{};
	float m_Distance = 0.f;

public:
	Mesh();
	~Mesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size);

	void SetDistance(float Distance) { m_Distance = Distance; }

	DirectX::BoundingBox GetBoundingBox() { return m_BoundingBox; }
	float GetDistance() { return m_Distance; }

	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};

// �ؽ�ó ������ �ϴ� ������Ʈ�� ����ϴ� Mesh
class TextureMesh : public Mesh
{
public:
	TextureMesh();
	~TextureMesh();

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind);
};

// ������ ���� Mesh
class TerrainMesh : public Mesh
{
private:
	float m_HeightMapYPos[257][257]{};

public:
	TerrainMesh();
	~TerrainMesh();

	float GetHeightMapYPos(int x, int z) { return m_HeightMapYPos[x][z]; }

	void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Scale, int Width, int Length, BYTE* YPos);

	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};

// bin ���Ϸ� �ε��� ������Ʈ�� ����ϴ� Mesh
class LoadedMesh : public Mesh
{
protected:
	// �� �������� �� ��ǥ
	ID3D12Resource *m_PositionBuffer = nullptr;
	ID3D12Resource *m_UploadPositionBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_PositionBufferView{};

	// �ؽ�ó ������ �� UV ��ǥ
	ID3D12Resource *m_UvBuffer = nullptr;
	ID3D12Resource *m_UploadUvBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_UvBufferView{};

	// ��� ���ο� ���� Normal, Tangent, BiTangent - 3x3 ��� (Normal Map�� �ִ� ���� Tangent, BiTangent���� ���)
	ID3D12Resource *m_NormalBuffer = nullptr;
	ID3D12Resource *m_UploadNormalBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_NormalBufferView{};

	ID3D12Resource *m_TangentBuffer = nullptr;
	ID3D12Resource *m_UploadTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_TangentBufferView{};

	ID3D12Resource *m_BiTangentBuffer = nullptr;
	ID3D12Resource *m_UploadBiTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_BiTangentBufferView{};

	char m_MeshName[64]{};

	DirectX::XMFLOAT3 m_AabbCenter{};
	DirectX::XMFLOAT3 m_AAbbExtent{};

	DirectX::XMFLOAT3 *m_Position = nullptr;
	DirectX::XMFLOAT4 m_Color{};
	DirectX::XMFLOAT2 *m_Uv0 = nullptr;
	DirectX::XMFLOAT2 m_Uv1{};
	DirectX::XMFLOAT3 *m_Normal = nullptr;
	DirectX::XMFLOAT3 *m_Tangent = nullptr;
	DirectX::XMFLOAT3 *m_BiTangent = nullptr;
	UINT *m_MeshIndex = nullptr;

	UINT m_SubMeshIndex = 0;

public:
	LoadedMesh();
	~LoadedMesh();

	void LoadMeshInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);

	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};

// bin ���Ϸ� �ε��� ������Ʈ �� Skin�� �ִ� ������Ʈ�� ����ϴ� Mesh
class SkinnedMesh : public Mesh
{
private:
	DirectX::XMFLOAT4X4 *m_BindPoseBoneOffset = nullptr;
	DirectX::XMFLOAT4X4 *m_BoneOffset = nullptr;
	ID3D12Resource *m_BoneOffsetBuffer = nullptr;

	DirectX::XMFLOAT4X4 *m_BoneTransform = nullptr;
	ID3D12Resource *m_BoneTransformBuffer = nullptr;

	DirectX::XMUINT4 *m_BoneIndex = nullptr;
	ID3D12Resource *m_BoneIndexBuffer = nullptr;
	ID3D12Resource *m_UploadBoneIndexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_BoneIndexBufferView{};

	DirectX::XMFLOAT4 *m_BoneWeight = nullptr;
	ID3D12Resource *m_BoneWeightBuffer = nullptr;
	ID3D12Resource *m_UploadBoneWeightBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_BoneWeightBufferView{};

	int m_BonePerVertex = 0;

	int m_BoneCount = 0;
	char(*m_BoneName)[64] = nullptr;
	GameObject **m_BoneFrame = nullptr;

	// --------------------
	// �� �������� �� ��ǥ
	ID3D12Resource *m_PositionBuffer = nullptr;
	ID3D12Resource *m_UploadPositionBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_PositionBufferView{};

	// �ؽ�ó ������ �� UV ��ǥ
	ID3D12Resource *m_UvBuffer = nullptr;
	ID3D12Resource *m_UploadUvBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_UvBufferView{};

	// ��� ���ο� ���� Normal, Tangent, BiTangent - 3x3 ��� (Normal Map�� �ִ� ���� Tangent, BiTangent���� ���)
	ID3D12Resource *m_NormalBuffer = nullptr;
	ID3D12Resource *m_UploadNormalBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_NormalBufferView{};

	ID3D12Resource *m_TangentBuffer = nullptr;
	ID3D12Resource *m_UploadTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_TangentBufferView{};

	ID3D12Resource *m_BiTangentBuffer = nullptr;
	ID3D12Resource *m_UploadBiTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_BiTangentBufferView{};

	char m_MeshName[64]{};

	DirectX::XMFLOAT3 m_AabbCenter{};
	DirectX::XMFLOAT3 m_AAbbExtent{};

	DirectX::XMFLOAT3 *m_Position = nullptr;
	DirectX::XMFLOAT4 m_Color{};
	DirectX::XMFLOAT2 *m_Uv0 = nullptr;
	DirectX::XMFLOAT2 m_Uv1{};
	DirectX::XMFLOAT3 *m_Normal = nullptr;
	DirectX::XMFLOAT3 *m_Tangent = nullptr;
	DirectX::XMFLOAT3 *m_BiTangent = nullptr;
	UINT *m_MeshIndex = nullptr;

	UINT m_SubMeshIndex = 0;
	// --------------------

public:
	SkinnedMesh();
	~SkinnedMesh();

	void CreateShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void LoadSkinInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);

	void SetBoneFrame(int Index, GameObject* Frame);

	int GetBoneCount() { return m_BoneCount; }
	char* GetBoneName(int Index) { return m_BoneName[Index]; }

	void UpdateShaderBuffer(ID3D12GraphicsCommandList* CommandList);
	virtual void Render(ID3D12GraphicsCommandList* CommandList);

	// --------------------
	void LoadMeshInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);
};