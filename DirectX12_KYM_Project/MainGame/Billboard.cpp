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

	if (m_GeometryBlob != nullptr) m_GeometryBlob->Release();

	if (m_Texture != nullptr) delete m_Texture;

	if (m_ObjectsWorldPos != nullptr) delete[] m_ObjectsWorldPos;
}

void Billboard::CreateShader(ID3D12Device* Device, ID3D12RootSignature* RootSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineStateDesc;
	ZeroMemory(&GraphicsPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	GraphicsPipelineStateDesc.pRootSignature = RootSignature;
	GraphicsPipelineStateDesc.InputLayout = CreateInputLayout();
	GraphicsPipelineStateDesc.RasterizerState = CreateRasterizerState();
	GraphicsPipelineStateDesc.BlendState = CreateBlendState();
	GraphicsPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	GraphicsPipelineStateDesc.VS = CreateVertexShader();
	GraphicsPipelineStateDesc.GS = CreateGeometryShader();
	GraphicsPipelineStateDesc.PS = CreatePixelShader();
	GraphicsPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	GraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	GraphicsPipelineStateDesc.SampleMask = UINT_MAX;
	GraphicsPipelineStateDesc.SampleDesc.Count = 1;
	GraphicsPipelineStateDesc.NumRenderTargets = 1;
	GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	GraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Device->CreateGraphicsPipelineState(&GraphicsPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_PipelineState);
}

void Billboard::CreateBillboard(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, Terrain* SceneTerrain, int Kind, int ObjectsCount)
{
	// Create Billboard Model - Create Mesh & Texture
	TextureVertex ModelVertex[1];
	ModelVertex[0] = TextureVertex(DirectX::XMFLOAT3(0.f, 0.f, 0.f), DirectX::XMFLOAT2(0.f, 0.f), Kind);

	m_VertexBuffer = CreateBuffer(Device, CommandList, ModelVertex, sizeof(TextureVertex), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_UploadVertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(TextureVertex);
	m_VertexBufferView.SizeInBytes = sizeof(TextureVertex);

	int TextureKind = Kind;

	if (Kind >= T_WALL && Kind < T_WALL + 4) TextureKind = T_WALL;

	m_Texture = new Texture();
	m_Texture->CreateTexture(Device, CommandList, nullptr, TextureKind, 1, 3);

	// Set Instancing Objects Information
	m_ObjectsCount = ObjectsCount;
	m_ObjectsWorldPos = new DirectX::XMFLOAT4X4[m_ObjectsCount];

	int ObjectIndex = 0;
	float ObjectX = 0.f, ObjectY = 0.f, ObjectZ = 0.f;

	int ObjectsCountSquareRoot = sqrt(ObjectsCount);

	// 오브젝트의 종류에 맞게 월드 좌표를 설정
	switch (Kind) {
	case T_BILLBOARDGRASS:
	case T_BILLBOARDTREE:
	{
		for (int z = 0; z < ObjectsCountSquareRoot; ++z) {
			for (int x = 0; x < ObjectsCountSquareRoot; ++x) {
				ObjectX = 150.f + rand() % 70 + (190.f * x);
				ObjectZ = 150.f + rand() % 70 + (190.f * z);

				int GetHeightMapX = int(ObjectX) / MAP_SCALE, GetHeightMapZ = int(ObjectZ) / MAP_SCALE;

				switch (Kind)
				{
				case T_BILLBOARDGRASS:
				{
					ObjectY = SceneTerrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ) + 2.5f;
				}
				break;

				case T_BILLBOARDTREE:
				{
					ObjectY = SceneTerrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ) + 80.f;
				}
				break;
				}
				m_ObjectsWorldPos[ObjectIndex++] = DirectX::XMFLOAT4X4(
					1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					ObjectX, ObjectY, ObjectZ, 1.f);
			}
		}
	}
	break;

	case T_WALL:
	case T_WALL + 1:
	case T_WALL + 2:
	case T_WALL + 3:
	{
		for (int i = 0; i < m_ObjectsCount; ++i) {
			switch (Kind) {
			case T_WALL: // x : 0 - 5000, z : 5000
			{
				ObjectX = 150.f + (200.f * i), ObjectY = 150.f, ObjectZ = 5050.f;
				if (i > 24) ObjectX -= 5000.f, ObjectY += 200.f;
				if (i > 49) ObjectX -= 5000.f, ObjectY += 200.f;
			}
			break;

			case T_WALL + 1: // x : 0 - 5000, z : 0
			{
				ObjectX = 150.f + (200.f * i), ObjectY = 150.f, ObjectZ = 50.f;
				if (i > 24) ObjectX -= 5000.f, ObjectY += 200.f;
				if (i > 49) ObjectX -= 5000.f, ObjectY += 200.f;
			}
			break;

			case T_WALL + 2: // x : 0, z : 0 - 5000
			{
				ObjectX = 50.f, ObjectY = 150.f, ObjectZ = 150.f + (200.f * i);
				if (i > 24) ObjectZ -= 5000.f, ObjectY += 200.f;
				if (i > 49) ObjectZ -= 5000.f, ObjectY += 200.f;
			}
			break;

			case T_WALL + 3: // x : 5000, z : 0 - 5000
			{
				ObjectX = 5050.f, ObjectY = 150.f, ObjectZ = 150.f + (200.f * i);
				if (i > 24) ObjectZ -= 5000.f, ObjectY += 200.f;
				if (i > 49) ObjectZ -= 5000.f, ObjectY += 200.f;
			}
			break;
			}
			m_ObjectsWorldPos[i] = DirectX::XMFLOAT4X4(
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				ObjectX, ObjectY, ObjectZ, 1.f);
		}
	}
	break;
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
	D3D12_INPUT_ELEMENT_DESC *InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[7];
	InputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[1] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[2] = { "KIND", 0, DXGI_FORMAT_R32_UINT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[3] = { "TRANSFORMPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[4] = { "TRANSFORMPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[5] = { "TRANSFORMPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	InputElementDesc[6] = { "TRANSFORMPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	ZeroMemory(&InputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	InputLayoutDesc.pInputElementDescs = InputElementDesc;
	InputLayoutDesc.NumElements = 7;

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

D3D12_SHADER_BYTECODE Billboard::CreateGeometryShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "BillboardGS", "gs_5_1", 0, 0, &m_GeometryBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_GeometryBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_GeometryBlob->GetBufferSize();

	return ShaderByteCode;
}

D3D12_SHADER_BYTECODE Billboard::CreatePixelShader()
{
	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "InstancingTexturePS", "ps_5_1", 0, 0, &m_PixelBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_PixelBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_PixelBlob->GetBufferSize();

	return ShaderByteCode;
}

void Billboard::Render(ID3D12GraphicsCommandList* CommandList)
{
	Shader::Render(CommandList);

	CommandList->SetGraphicsRoot32BitConstants(1, 4, &m_ChangeTexcoords, 32);

	if (m_Texture != nullptr) m_Texture->Render(CommandList, 0);

	for (int i = 0; i < m_ObjectsCount; ++i)
		DirectX::XMStoreFloat4x4(&m_MappingInstanceData[i].m_TransformPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ObjectsWorldPos[i])));

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[2] = { m_VertexBufferView, m_InstanceBufferView };
	CommandList->IASetVertexBuffers(0, 2, VertexBufferView);
	CommandList->DrawInstanced(1, m_ObjectsCount, 0, 0);
}