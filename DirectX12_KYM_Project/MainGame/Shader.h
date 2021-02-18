#pragma once
// HLSL과 그래픽스 파이프라인을 연결
class Shader
{
private:
	ID3D12PipelineState *m_PipelineState;

public:
	Shader();
	~Shader();

	void CreateShader(ID3D12Device* &Device, ID3D12RootSignature* &RootSignature);

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_RASTERIZER_DESC CreateRasterizerState();
	D3D12_BLEND_DESC CreateBlendState();
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_SHADER_BYTECODE CreateVertexShader();
	D3D12_SHADER_BYTECODE CreatePixelShader();
};