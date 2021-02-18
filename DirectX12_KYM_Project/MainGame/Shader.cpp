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
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // 채우기 모드를 설정
	RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; // 그리지 않은 면의 방향을 설정 - 뒤
	RasterizerDesc.FrontCounterClockwise = false; // false - 와인딩 오더를 반시계 방향으로 설정
	RasterizerDesc.DepthBias = 0; // 픽셀의 깊이 값에 더해질 깊이 바이어스 값
	RasterizerDesc.DepthBiasClamp = 0.f; // 픽셀 깊이 값에 더해질 최대 값
	RasterizerDesc.SlopeScaledDepthBias = 0.f; // 픽셀 기울기에 따라 바이어스를 조절할 값
	RasterizerDesc.DepthClipEnable = true; // 깊이 값을 사용하여 클리핑 사용
	RasterizerDesc.MultisampleEnable = false; // 다중 샘플링 활성화
	RasterizerDesc.AntialiasedLineEnable = false; // 선분 안티에일리언싱 활성화
	RasterizerDesc.ForcedSampleCount = 0; // UAV 렌더링에 사용할 샘플의 개수를 결정
	RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; // 프리미티브에 조금이라도 걸치는 픽셀의 래스터라이제이션 유무 결정 - 보수적 래스터라이제이션

	return RasterizerDesc;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = false; // 다중 샘플링을 위해 렌더타겟 0의 알파 값을 커버리지 매스크로 변환
	BlendDesc.IndependentBlendEnable = false; // 각 렌더타겟에서 독립적인 블랜딩 - false이면 0번만 사용
	BlendDesc.RenderTarget[0].BlendEnable = false; // 블렌딩 활성화
	BlendDesc.RenderTarget[0].LogicOpEnable = false; // 논리 연산 활성화
	BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; // 픽셀 색상에 곱하는 값 - 요소별 연산
	BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; // 렌더타겟 색상에 곱하는 값
	BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // RGB 색상 블렌드 연산자
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 블렌드 타겟에게 적용할 마스크

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