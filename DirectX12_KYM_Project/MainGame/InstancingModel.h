#pragma once

#include "Shader.h"

class GameObject;

struct MAPPING_INSTANCE
{
	DirectX::XMFLOAT4X4 m_TransformPos;
};

// Scene에서 많은 수로 등장하는 오브젝트를 인스턴싱 처리
class InstancingModel : public Shader
{
protected:
	ID3D12Resource *m_InstanceBuffer = nullptr;
	MAPPING_INSTANCE *m_MappingInstanceData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_InstanceBufferView;

	std::vector<GameObject*> m_InstancingObject{};

public:
	InstancingModel();
	~InstancingModel();

	virtual void CreateModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int ModelCount) {}
	void CreateInstanceBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int ModelCount);

	void UpdateInstanceBuffer();
	virtual void Animate(float ElapsedTime) {}
	virtual void Render(ID3D12GraphicsCommandList* CommandList);
};

// bin 파일로 로드된 모델을 인스턴싱 처리
class InstancingLoadedModel : public InstancingModel
{
public:
	InstancingLoadedModel();
	~InstancingLoadedModel();

	virtual void CreateModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int ModelCount) {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin 파일로 로드된 모델 중 Skin이 있는 모델의 인스턴싱 처리
class InstancingSkinnedModel : public InstancingLoadedModel
{
public:
	InstancingSkinnedModel();
	~InstancingSkinnedModel();

	virtual void CreateModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int ModelCount);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();

	virtual void Animate(float ElapsedTime);
};