#pragma once
// HLSL과 그래픽스 파이프라인을 연결
class Shader
{
protected:
	ID3D12PipelineState *m_PipelineState = nullptr;

	ID3DBlob *m_VertexBlob = nullptr;
	ID3DBlob *m_PixelBlob = nullptr;

public:
	Shader();
	~Shader();

	void CreateShader(ID3D12Device* Device, ID3D12RootSignature* RootSignature);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	void Render(ID3D12GraphicsCommandList* CommandList);
};


class TextureShader : public Shader
{
public:
	TextureShader() {}
	~TextureShader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};