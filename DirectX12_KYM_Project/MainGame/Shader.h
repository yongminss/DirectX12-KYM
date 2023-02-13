#pragma once

struct MAPPING_INSTANCE
{
	DirectX::XMFLOAT4X4 m_TransformPos;
};

// HLSL�� �׷��Ƚ� ������������ ����
class Shader
{
protected:
	ID3D12PipelineState *m_PipelineState = nullptr;

	ID3DBlob *m_VertexBlob = nullptr;
	ID3DBlob *m_PixelBlob = nullptr;

	int m_Kind = 0;

public:
	Shader();
	~Shader();

	void CreateShader(ID3D12Device* Device, ID3D12RootSignature* RootSignature, int Kind);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	void Render(ID3D12GraphicsCommandList* CommandList);
};

// Texture�� �ִ� ������Ʈ�� ����� Shader
class TextureShader : public Shader
{
public:
	TextureShader() { }
	~TextureShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// �Ҳ� ȿ���� ����� Shader
class FlameShader : public Shader
{
public:
	FlameShader() { }
	~FlameShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// ������ ����� Shader
class TerrainShader : public Shader
{
public:
	TerrainShader() { }
	~TerrainShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin ���Ϸ� �ε��� ������Ʈ�� ����ϴ� Shader
class LoadedShader : public Shader
{
public:
	LoadedShader() { }
	~LoadedShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin ���Ϸ� �ε��� ������Ʈ �� Skin�� �ִ� ������Ʈ�� ����ϴ� Shader
class SkinnedShader : public LoadedShader
{
public:
	SkinnedShader() { }
	~SkinnedShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};