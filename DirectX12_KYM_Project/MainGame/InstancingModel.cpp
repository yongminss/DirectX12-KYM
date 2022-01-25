#include "stdafx.h"
#include "InstancingModel.h"
#include "GameObject.h"


InstancingModel::InstancingModel()
{

}

InstancingModel::~InstancingModel()
{
	if (m_InstanceBuffer != nullptr) m_InstanceBuffer->Release();

	/*for (std::vector<GameObject*>::iterator Iter = m_InstancingObject.begin(); Iter != m_InstancingObject.end(); ++Iter) {
		delete *Iter;
		Iter = m_InstancingObject.erase(Iter);
	}*/
}

void InstancingModel::CreateInstanceBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int ModelCount)
{
	m_InstanceBuffer = CreateBuffer(Device, CommandList, nullptr, sizeof(MAPPING_INSTANCE) * ModelCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_InstanceBuffer->Map(0, nullptr, (void**)&m_MappingInstanceData);

	m_InstanceBufferView.BufferLocation = m_InstanceBuffer->GetGPUVirtualAddress();
	m_InstanceBufferView.StrideInBytes = sizeof(MAPPING_INSTANCE);
	m_InstanceBufferView.SizeInBytes = sizeof(MAPPING_INSTANCE) * ModelCount;
}

void InstancingModel::UpdateInstanceBuffer()
{
	for (int i = 0; i < m_InstancingObject.size(); ++i)
		DirectX::XMStoreFloat4x4(&m_MappingInstanceData[i].m_TransformPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_InstancingObject[i]->GetWorldPos())));
}

void InstancingModel::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (m_InstancingObject.size() != 0) {
		Shader::Render(CommandList);

		UpdateInstanceBuffer();

		m_InstancingObject[0]->Render(CommandList, m_InstanceBufferView, m_InstancingObject.size());
	}
}

// --------------------
InstancingLoadedModel::InstancingLoadedModel()
{

}

InstancingLoadedModel::~InstancingLoadedModel()
{

}

D3D12_INPUT_LAYOUT_DESC InstancingLoadedModel::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC *InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[9];
	InputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[1] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[5] = { "TRANSFORMPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[6] = { "TRANSFORMPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[7] = { "TRANSFORMPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[8] = { "TRANSFORMPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	ZeroMemory(&InputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	InputLayoutDesc.pInputElementDescs = InputElementDesc;
	InputLayoutDesc.NumElements = 9;

	return InputLayoutDesc;
}

D3D12_SHADER_BYTECODE InstancingLoadedModel::CreateVertexShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "InstanceLoadedVS", "vs_5_1", 0, 0, &m_VertexBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_VertexBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_VertexBlob->GetBufferSize();

	return ShaderByteCode;
}

D3D12_SHADER_BYTECODE InstancingLoadedModel::CreatePixelShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "LoadedPS", "ps_5_1", 0, 0, &m_PixelBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_PixelBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_PixelBlob->GetBufferSize();

	return ShaderByteCode;
}

// --------------------
InstancingSkinnedModel::InstancingSkinnedModel()
{

}

InstancingSkinnedModel::~InstancingSkinnedModel()
{

}

void InstancingSkinnedModel::CreateModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int ModelCount)
{
	m_InstancingObject.reserve(ModelCount);

	GameObject *UsingGameObject = GameObject::LoadBinaryFileModel(Device, CommandList, RootSignature, "Model/Monster_WeakOrc.bin", this, true);

	for (int i = 0; i < ModelCount; ++i) {
		m_InstancingObject.emplace_back(new GameObject());
		m_InstancingObject.back()->SetChild(UsingGameObject);
		m_InstancingObject.back()->SetPosition(DirectX::XMFLOAT3(0.f + (i * 100.f), 0.f, 0.f));
		m_InstancingObject.back()->SetScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	}
	CreateInstanceBuffer(Device, CommandList, ModelCount);
}

D3D12_INPUT_LAYOUT_DESC InstancingSkinnedModel::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC *InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[11];
	InputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[1] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[5] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[6] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	InputElementDesc[7] = { "TRANSFORMPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[8] = { "TRANSFORMPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[9] = { "TRANSFORMPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[10] = { "TRANSFORMPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 7, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	ZeroMemory(&InputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	InputLayoutDesc.pInputElementDescs = InputElementDesc;
	InputLayoutDesc.NumElements = 11;

	return InputLayoutDesc;
}

D3D12_SHADER_BYTECODE InstancingSkinnedModel::CreateVertexShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "InstanceSkinnedVS", "vs_5_1", 0, 0, &m_VertexBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_VertexBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_VertexBlob->GetBufferSize();

	return ShaderByteCode;
}

void InstancingSkinnedModel::Animate(float ElapsedTime)
{
	if (m_InstancingObject.size() != 0) {
		m_InstancingObject[0]->Animate(ElapsedTime);
		m_InstancingObject[0]->UpdateTransform(nullptr);
	}
}