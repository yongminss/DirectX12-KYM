#pragma once

class GameObject;

// 오브젝트가 사용할 Vertex Buffer 생성 및 관리
class Mesh
{
protected:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	ID3D12Resource *m_IndexBuffer = nullptr;
	ID3D12Resource *m_UploadIndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView{};

	int m_VertexCount = 0;
	int m_IndexCount = 0;

	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	char m_MeshName[64]{};

	float m_Distance = 0.f;
	DirectX::BoundingBox m_BoundingBox{};

public:
	Mesh();
	Mesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, float Size);
	~Mesh();

	void SetDistance(float Distance) { m_Distance = Distance; }

	char* GetMeshName() { return m_MeshName; }
	float GetDistance() { return m_Distance; }
	DirectX::BoundingBox GetBoundingBox() { return m_BoundingBox; }

	virtual void UpdateShaderBuffer(ID3D12GraphicsCommandList* CommandList) {}
	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList);
	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView) {}
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
	virtual void Render(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView, int ModelCount);
};

// 텍스처 매핑을 하는 오브젝트가 사용하는 Mesh
class TextureMesh : public Mesh
{
public:
	TextureMesh();
	TextureMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Size, int Kind);
	~TextureMesh();
};

// 지형에 사용될 Mesh
class TerrainMesh : public Mesh
{
private:
	float m_HeightMapYPos[257][257]{};

public:
	TerrainMesh();
	TerrainMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Scale, int Width, int Length, BYTE* YPos);
	~TerrainMesh();

	float GetHeightMapYPos(int x, int z) { return m_HeightMapYPos[x][z]; }
};

// bin 파일로 로드한 오브젝트가 사용하는 Mesh
class LoadedMesh : public Mesh
{
protected:
	// 각 프레임의 모델 좌표
	ID3D12Resource *m_PositionBuffer = nullptr;
	ID3D12Resource *m_UploadPositionBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_PositionBufferView{};

	// 텍스처 매핑이 될 UV 좌표
	ID3D12Resource *m_UvBuffer = nullptr;
	ID3D12Resource *m_UploadUvBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_UvBufferView{};

	// 노멀 매핑에 사용될 Normal, Tangent, BiTangent - 3x3 행렬 (Normal Map이 있는 모델은 Tangent, BiTangent까지 사용)
	ID3D12Resource *m_NormalBuffer = nullptr;
	ID3D12Resource *m_UploadNormalBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_NormalBufferView{};

	ID3D12Resource *m_TangentBuffer = nullptr;
	ID3D12Resource *m_UploadTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_TangentBufferView{};

	ID3D12Resource *m_BiTangentBuffer = nullptr;
	ID3D12Resource *m_UploadBiTangentBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_BiTangentBufferView{};

	DirectX::XMFLOAT3 m_AabbCenter{};
	DirectX::XMFLOAT3 m_AAbbExtent{};

	DirectX::XMFLOAT3 *m_Position = nullptr;
	DirectX::XMFLOAT4 m_Color{};
	DirectX::XMFLOAT2 *m_Uv0 = nullptr;
	DirectX::XMFLOAT2 *m_Uv1 = nullptr;
	DirectX::XMFLOAT3 *m_Normal = nullptr;
	DirectX::XMFLOAT3 *m_Tangent = nullptr;
	DirectX::XMFLOAT3 *m_BiTangent = nullptr;
	UINT *m_MeshIndex = nullptr;

	UINT m_SubMeshIndex = 0;

public:
	LoadedMesh();
	~LoadedMesh();

	void LoadMeshInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);

	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList);
	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView);
};

// bin 파일로 로드한 오브젝트 중 Skin이 있는 오브젝트가 사용하는 Mesh
class SkinnedMesh : public LoadedMesh
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

public:
	SkinnedMesh();
	~SkinnedMesh();

	void LoadSkinInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);
	void CreateShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void SetBoneFrame(int Index, GameObject* Frame);

	int GetBoneCount() { return m_BoneCount; }
	char* GetBoneName(int Index) { return m_BoneName[Index]; }

	virtual void UpdateShaderBuffer(ID3D12GraphicsCommandList* CommandList);
	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList);
	virtual void UpdateVertexBufferView(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView);
};