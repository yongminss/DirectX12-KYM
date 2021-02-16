#pragma once
// HLSL과 그래픽스 파이프라인을 연결
class Shader
{
private:


public:
	Shader();
	~Shader();

	void CreateShader(ID3D12RootSignature* &RootSignature);

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_RASTERIZER_DESC CreateRasterizerState();
	D3D12_BLEND_DESC CreateBlendState();
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
};