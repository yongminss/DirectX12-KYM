#pragma once

#include "Shader.h"
#include "Texture.h"

// Shader & Texture 를 관리 (ex. 생성, 삭제, 값의 변경 등)
class Material
{
private:
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

	void LoadMaterialInfo(FILE* File);
	void LoadTextureInfo(FILE* File);
};