#pragma once

class Shader;
class Texture;

// Shader & Texture 를 관리 (ex. 생성, 삭제, 값의 변경 등)
class Material
{
private:
	Shader *m_Shader = nullptr;
	Texture *m_Texture = nullptr;

	unsigned int m_MaterialCount = 0;

	DirectX::XMFLOAT4 m_Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	DirectX::XMFLOAT4 m_Diffuse = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	DirectX::XMFLOAT4 m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	DirectX::XMFLOAT4 m_Emissive = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	float m_Glossiness = 0.f;
	float m_Smoothness = 0.f;
	float m_Metallic = 0.f;
	float m_SpecularHighlight = 0.f;
	float m_GlossyReflection = 0.f;

public:
	Material();
	~Material();

	void CreateMaterial(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);

	void LoadMaterialInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File, int Type, Shader* InstanceShader);
	void LoadTextureInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File);

	void SetShader(Shader* ObjectShader) { m_Shader = ObjectShader; }
	void SetTexture(Texture* ObjectTexture) { m_Texture = ObjectTexture; }

	void SetPipeline(ID3D12GraphicsCommandList* CommandList);
	void MappingTexture(ID3D12GraphicsCommandList* CommandList, int Index);
};