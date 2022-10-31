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

	int m_Type = 0;

public:
	Shader();
	~Shader();

	void CreateShader(ID3D12Device* Device, ID3D12RootSignature* RootSignature, int Type = 0);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	void Render(ID3D12GraphicsCommandList* CommandList);
};

// UI�� ����� Shader
class UserInterfaceShader : public Shader
{
public:
	UserInterfaceShader() { }
	~UserInterfaceShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// ���� ���� �ؽ�ó�� ����� Shader
class MultipleTextureShader : public Shader
{
public:
	MultipleTextureShader() { }
	~MultipleTextureShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// ---
class EffectShader : public Shader
{
public:
	EffectShader() { }
	~EffectShader() { }

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
	LoadedShader() {}
	~LoadedShader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin ���Ϸ� �ε��� ������Ʈ �� Skin�� �ִ� ������Ʈ�� ����ϴ� Shader
class SkinnedShader : public LoadedShader
{
public:
	SkinnedShader() {}
	~SkinnedShader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};