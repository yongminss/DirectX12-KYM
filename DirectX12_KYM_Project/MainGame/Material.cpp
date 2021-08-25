#include "stdafx.h"
#include "Material.h"


Material::Material()
{

}

Material::~Material()
{
	if (m_Shader != nullptr) delete m_Shader;
	if (m_Texture != nullptr) delete m_Texture;
}

void Material::CreateMaterial(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	switch (Kind) {
	case 0: // Terrain
	{
		TerrainShader *UsingShader = new TerrainShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 2, 3);
		SetTexture(UsingTexture);
	}
	break;

	case 1: // Skybox
	{
		SkyboxShader *UsingShader = new SkyboxShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 6, 2);
		SetTexture(UsingTexture);
	}
	break;

	case 2: // UserInterface
	{
		UserInterfaceShader *UsingShader = new UserInterfaceShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, L"Texture/SkyBox_Bottom.dds", Kind, 1, 2);
		SetTexture(UsingTexture);
	}
	break;

	default:
	{
		Shader *UsingShader = new Shader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);
	}
	break;
	}
}

void Material::LoadMaterialInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File)
{
	fread(&m_MaterialCount, sizeof(int), 1, File);

	char Word[64] = { '\0' };
	BYTE WordCount = '\0';

	int MaterialCount = 0;

	while (true) {
		fread(&WordCount, sizeof(BYTE), 1, File);
		fread(Word, sizeof(char), WordCount, File);
		Word[WordCount] = '\0';

		if (!strcmp(Word, "<Material>:")) {
			fread(&MaterialCount, sizeof(int), 1, File);

			// Shader & Texture�� Set
			LoadedShader *UsingShader = new LoadedShader();
			UsingShader->CreateShader(Device, RootSignature);
			SetShader(UsingShader);
		}

		else if (!strcmp(Word, "<AlbedoColor>:")) {
			fread(&m_Albedo, sizeof(float), 4, File);
		}

		else if (!strcmp(Word, "<EmissiveColor>:")) {
			fread(&m_Emissive, sizeof(float), 4, File);
		}

		else if (!strcmp(Word, "<SpecularColor>:")) {
			fread(&m_Specular, sizeof(float), 4, File);
		}

		else if (!strcmp(Word, "<Glossiness>:")) {
			fread(&m_Glossiness, sizeof(float), 1, File);
		}

		else if (!strcmp(Word, "<Smoothness>:")) {
			fread(&m_Smoothness, sizeof(float), 1, File);
		}

		else if (!strcmp(Word, "<Metallic>:")) {
			fread(&m_Metallic, sizeof(float), 1, File);
		}

		else if (!strcmp(Word, "<SpecularHighlight>:")) {
			fread(&m_SpecularHighlight, sizeof(float), 1, File);
		}

		else if (!strcmp(Word, "<GlossyReflection>:")) {
			fread(&m_GlossyReflection, sizeof(float), 1, File);
		}

		else if (!strcmp(Word, "<AlbedoMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<SpecularMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<NormalMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<MetallicMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<EmissionMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<DetailAlbedoMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "<DetailNormalMap>:")) {
			LoadTextureInfo(Device, CommandList, File);
		}

		else if (!strcmp(Word, "</Materials>")) {
			break;
		}
	}
}

void Material::LoadTextureInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File)
{
	char TextureName[64] = { '\0' };
	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(TextureName, sizeof(char), WordCount, File);
	TextureName[WordCount] = '\0';

	if (strcmp(TextureName, "null")) {
		// �ؽ�ó�� �̸��� null�� �ƴ϶��, �ؽ�ó ������ �̸��� ���� - ���� ����, dds �� - (ex. Texture/~~~.dds)

		bool Duplicated = false;
		Duplicated = (TextureName[0] == '@'); // �ؽ�ó�� ù��° ���ڰ� @�̸� �̹� ���������Ƿ� �ߺ� �������� ����

		if (Duplicated == false) {
			char FilePath[64] = { '\0' };
			strcpy_s(FilePath, 64, "Model/Texture/");
			strcat_s(FilePath, TextureName);
			strcat_s(FilePath, ".dds");

			size_t ConvertCount = 0;
			wchar_t* FrameTextureName = new wchar_t[64];
			mbstowcs_s(&ConvertCount, FrameTextureName, 64, FilePath, _TRUNCATE);

			// Texture�� ����
			Texture *UsingTexture = new Texture();
			UsingTexture->CreateTexture(Device, CommandList, FrameTextureName, -1, 1, 4);
			SetTexture(UsingTexture);

			delete[] FrameTextureName;
		}
	}
}

void Material::SetPipeline(ID3D12GraphicsCommandList* CommandList)
{
	if (m_Shader != nullptr) m_Shader->Render(CommandList);
}

void Material::MappingTexture(ID3D12GraphicsCommandList* CommandList, int Index)
{
	if (m_Texture != nullptr) m_Texture->Render(CommandList, Index);
}