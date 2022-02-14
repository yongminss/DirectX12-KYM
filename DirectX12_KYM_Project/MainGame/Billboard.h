#pragma once

#include "Shader.h"

class Texture;
class Terrain;

class Billboard : public Shader
{
private:
	ID3D12Resource *m_VertexBuffer = nullptr;
	ID3D12Resource *m_UploadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	ID3D12Resource *m_InstanceBuffer = nullptr;
	MAPPING_INSTANCE *m_MappingInstanceData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_InstanceBufferView{};

	Texture *m_Texture = nullptr;

	int m_ObjectsCount = 0;
	DirectX::XMFLOAT4X4 *m_ObjectsWorldPos = nullptr;

public:
	Billboard();
	~Billboard();

	void CreateBillboard(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, Terrain* SceneTerrain, int ObjectsCount);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	void Render(ID3D12GraphicsCommandList* CommandList);
};