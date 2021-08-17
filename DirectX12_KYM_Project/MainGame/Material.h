#pragma once

#include "Shader.h"
#include "Texture.h"

// Shader & Texture �� ���� (ex. ����, ����, ���� ���� ��)
class Material
{
private:
	Shader *m_Shader = nullptr;
	Texture *m_Texture = nullptr;

	unsigned int m_MaterialCount = 0;

	DirectX::XMFLOAT4 m_Albedo{};
	DirectX::XMFLOAT4 m_Emissive{};
	DirectX::XMFLOAT4 m_Specular{};
	float m_Glossiness = 0.f;
	float m_Smoothness = 0.f;
	float m_Metallic = 0.f;
	float m_SpecularHighlight = 0.f;
	float m_GlossyReflection = 0.f;

public:
	Material();
	~Material();

	void CreateMaterial(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind);

	void LoadMaterialInfo(ID3D12Device* Device, ID3D12RootSignature* RootSignature, FILE* File);
	void LoadTextureInfo(FILE* File);

	void SetShader(Shader* ObjectShader) { m_Shader = ObjectShader; }
	void SetTexture(Texture* ObjectTexture) { m_Texture = ObjectTexture; }

	void SetPipeline(ID3D12GraphicsCommandList* CommandList);
	void MappingTexture(ID3D12GraphicsCommandList* CommandList, int Index);
};