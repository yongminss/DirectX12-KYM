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

// UI에 사용할 Shader
class UserInterfaceShader : public Shader
{
public:
	UserInterfaceShader() { }
	~UserInterfaceShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// Skybox에 사용할 Shader
class SkyboxShader : public Shader
{
public:
	SkyboxShader() { }
	~SkyboxShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// 지형에 사용할 Shader
class TerrainShader : public Shader
{
public:
	TerrainShader() { }
	~TerrainShader() { }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin 파일로 로드한 오브젝트가 사용하는 Shader
class LoadedShader : public Shader
{
public:
	LoadedShader() {}
	~LoadedShader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

// bin 파일로 로드한 오브젝트 중 Skin이 있는 오브젝트가 사용하는 Shader
class SkinnedShader : public LoadedShader
{
public:
	SkinnedShader() {}
	~SkinnedShader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};