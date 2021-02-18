#include "stdafx.h"
#include "Shader.h"


Shader::Shader()
{

}

Shader::~Shader()
{
	if (m_PipelineState != nullptr) m_PipelineState->Release();
}

void Shader::CreateShader(ID3D12Device* &Device, ID3D12RootSignature* &RootSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineStateDesc;
	ZeroMemory(&GraphicsPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	GraphicsPipelineStateDesc.pRootSignature = RootSignature;
	GraphicsPipelineStateDesc.InputLayout = CreateInputLayout();
	GraphicsPipelineStateDesc.RasterizerState = CreateRasterizerState();
	GraphicsPipelineStateDesc.BlendState = CreateBlendState();
	GraphicsPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	GraphicsPipelineStateDesc.VS = CreateVertexShader();
	GraphicsPipelineStateDesc.PS = CreatePixelShader();
	GraphicsPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	GraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GraphicsPipelineStateDesc.SampleMask = UINT_MAX;
	GraphicsPipelineStateDesc.SampleDesc.Count = 1;
	GraphicsPipelineStateDesc.NumRenderTargets = 1;
	GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	GraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Device->CreateGraphicsPipelineState(&GraphicsPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_PipelineState);
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	ZeroMemory(&InputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	InputLayoutDesc.pInputElementDescs = nullptr;
	InputLayoutDesc.NumElements = 0;

	return InputLayoutDesc;
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory(&RasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // ä��� ��带 ����
	RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; // �׸��� ���� ���� ������ ���� - ��
	RasterizerDesc.FrontCounterClockwise = false; // false - ���ε� ������ �ݽð� �������� ����
	RasterizerDesc.DepthBias = 0; // �ȼ��� ���� ���� ������ ���� ���̾ ��
	RasterizerDesc.DepthBiasClamp = 0.f; // �ȼ� ���� ���� ������ �ִ� ��
	RasterizerDesc.SlopeScaledDepthBias = 0.f; // �ȼ� ���⿡ ���� ���̾�� ������ ��
	RasterizerDesc.DepthClipEnable = true; // ���� ���� ����Ͽ� Ŭ���� ���
	RasterizerDesc.MultisampleEnable = false; // ���� ���ø� Ȱ��ȭ
	RasterizerDesc.AntialiasedLineEnable = false; // ���� ��Ƽ���ϸ���� Ȱ��ȭ
	RasterizerDesc.ForcedSampleCount = 0; // UAV �������� ����� ������ ������ ����
	RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; // ������Ƽ�꿡 �����̶� ��ġ�� �ȼ��� �����Ͷ������̼� ���� ���� - ������ �����Ͷ������̼�

	return RasterizerDesc;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = false; // ���� ���ø��� ���� ����Ÿ�� 0�� ���� ���� Ŀ������ �Ž�ũ�� ��ȯ
	BlendDesc.IndependentBlendEnable = false; // �� ����Ÿ�ٿ��� �������� ���� - false�̸� 0���� ���
	BlendDesc.RenderTarget[0].BlendEnable = false; // ���� Ȱ��ȭ
	BlendDesc.RenderTarget[0].LogicOpEnable = false; // �� ���� Ȱ��ȭ
	BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; // �ȼ� ���� ���ϴ� �� - ��Һ� ����
	BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; // ����Ÿ�� ���� ���ϴ� ��
	BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // RGB ���� ���� ������
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // ���� Ÿ�ٿ��� ������ ����ũ

	return BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	DepthStencilDesc.DepthEnable = false;
	DepthStencilDesc.StencilEnable = false;

	return DepthStencilDesc;
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader()
{
	ID3DBlob *m_VertexBlob = nullptr;

	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "VS", "vs_5_1", 0, 0, &m_VertexBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_VertexBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_VertexBlob->GetBufferSize();

	if (m_VertexBlob != nullptr) m_VertexBlob->Release();

	return ShaderByteCode;
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader()
{
	ID3DBlob *m_PixelBlob = nullptr;

	D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "PS", "ps_5_1", 0, 0, &m_PixelBlob, nullptr);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.pShaderBytecode = m_PixelBlob->GetBufferPointer();
	ShaderByteCode.BytecodeLength = m_PixelBlob->GetBufferSize();

	if (m_PixelBlob != nullptr) m_PixelBlob->Release();

	return ShaderByteCode;
}