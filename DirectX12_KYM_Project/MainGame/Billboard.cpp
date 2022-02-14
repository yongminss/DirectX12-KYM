#include "stdafx.h"
#include "Billboard.h"

#include "Vertex.h"
#include "Texture.h"
#include "Terrain.h"


Billboard::Billboard()
{

}

Billboard::~Billboard()
{
	if (m_VertexBuffer != nullptr) m_VertexBuffer->Release();
	if (m_UploadVertexBuffer != nullptr) m_UploadVertexBuffer->Release();

	if (m_InstanceBuffer != nullptr) m_InstanceBuffer->Release();

	if (m_Texture != nullptr) delete m_Texture;
	if (m_ObjectsWorldPos != nullptr) delete[] m_ObjectsWorldPos;
}

void Billboard::CreateBillboard(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, Terrain* SceneTerrain, int ObjectsCount)
{
	// Create Billboard Model (Grass) - Create Mesh & Texture
	float TextureSize = 10.f;
	TextureVertex ModelVertex[6];

	ModelVertex[0] = TextureVertex(DirectX::XMFLOAT3(+TextureSize * 2.f, +TextureSize, TextureSize), DirectX::XMFLOAT2(1.f, 0.f));
	ModelVertex[1] = TextureVertex(DirectX::XMFLOAT3(+TextureSize * 2.f, -TextureSize, TextureSize), DirectX::XMFLOAT2(1.f, 1.f));
	ModelVertex[2] = TextureVertex(DirectX::XMFLOAT3(-TextureSize * 2.f, -TextureSize, TextureSize), DirectX::XMFLOAT2(0.f, 1.f));

	ModelVertex[3] = TextureVertex(DirectX::XMFLOAT3(-TextureSize * 2.f, -TextureSize, TextureSize), DirectX::XMFLOAT2(0.f, 1.f));
	ModelVertex[4] = TextureVertex(DirectX::XMFLOAT3(-TextureSize * 2.f, +TextureSize, TextureSize), DirectX::XMFLOAT2(0.f, 0.f));
	ModelVertex[5] = TextureVertex(DirectX::XMFLOAT3(+TextureSize * 2.f, +TextureSize, TextureSize), DirectX::XMFLOAT2(1.f, 0.f));

	m_VertexBuffer = CreateBuffer(Device, CommandList, ModelVertex, sizeof(TextureVertex) * 6, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(TextureVertex);
	m_VertexBufferView.SizeInBytes = sizeof(TextureVertex) * 6;

	m_Texture = new Texture();
	m_Texture->CreateTexture(Device, CommandList, nullptr, T_GRASS, 1, 2);

	// Set Instancing Objects Information
	m_ObjectsCount = ObjectsCount;
	m_ObjectsWorldPos = new DirectX::XMFLOAT4X4[m_ObjectsCount];

	float ObjectX = 0.f, ObjectY = 0.f, ObjectZ = 0.f;

	for (int i = 0; i < m_ObjectsCount; ++i) {
		ObjectX = rand() % 5000, ObjectZ = rand() % 5000;
		int GetHeightMapX = int(ObjectX) / MAP_SCALE, GetHeightMapZ = int(ObjectZ) / MAP_SCALE;
		ObjectY = SceneTerrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ) + TextureSize;

		m_ObjectsWorldPos[i] = DirectX::XMFLOAT4X4(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			ObjectX, ObjectY, ObjectZ, 1.f);
	}

	// Create Instance Buffer
	m_InstanceBuffer = CreateBuffer(Device, CommandList, nullptr, sizeof(MAPPING_INSTANCE) * m_ObjectsCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_InstanceBuffer->Map(0, nullptr, (void**)&m_MappingInstanceData);

	m_InstanceBufferView.BufferLocation = m_InstanceBuffer->GetGPUVirtualAddress();
	m_InstanceBufferView.StrideInBytes = sizeof(MAPPING_INSTANCE);
	m_InstanceBufferView.SizeInBytes = sizeof(MAPPING_INSTANCE) * m_ObjectsCount;
}

D3D12_INPUT_LAYOUT_DESC Billboard::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC *InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[6];
	InputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[1] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[2] = { "TRANSFORMPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[3] = { "TRANSFORMPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[4] = { "TRANSFORMPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[5] = { "TRANSFORMPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	ZeroMemory(&InputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	InputLayoutDesc.pInputElementDescs = InputElementDesc;
	InputLayoutDesc.NumElements = 6;

	return InputLayoutDesc;
}

D3D12_SHADER_BYTECODE Billboard::CreateVertexShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "InstancingTextureVS", "vs_5_1", 0, 0, &m_VertexBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_VertexBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_VertexBlob->GetBufferSize();

	return ShaderByteCode;
}

D3D12_SHADER_BYTECODE Billboard::CreatePixelShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "TexturePS", "ps_5_1", 0, 0, &m_PixelBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_PixelBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_PixelBlob->GetBufferSize();

	return ShaderByteCode;
}

void Billboard::Render(ID3D12GraphicsCommandList* CommandList)
{
	Shader::Render(CommandList);

	if (m_Texture != nullptr) m_Texture->Render(CommandList, 0);

	for (int i = 0; i < m_ObjectsCount; ++i)
		DirectX::XMStoreFloat4x4(&m_MappingInstanceData[i].m_TransformPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ObjectsWorldPos[i])));

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[2] = { m_VertexBufferView, m_InstanceBufferView };
	CommandList->IASetVertexBuffers(0, 2, VertexBufferView);
	CommandList->DrawInstanced(6, m_ObjectsCount, 0, 0);
}