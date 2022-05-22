#include "stdafx.h"
#include "Material.h"

#include "Shader.h"
#include "Texture.h"
#include "InstancingModel.h"
#include "GameObject.h"


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
	case T_TERRAIN: // Terrain
	{
		TerrainShader *UsingShader = new TerrainShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 2, 3);
		SetTexture(UsingTexture);
	}
	break;

	case T_SKYBOX: // Skybox
	{
		SkyboxShader *UsingShader = new SkyboxShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 6, 2);
		SetTexture(UsingTexture);
	}
	break;

	// 여기부터 UI로 사용하는 Texture
	case T_HPBAR: // Bar
	{
		UserInterfaceShader *UsingShader = new UserInterfaceShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 1, 2);
		SetTexture(UsingTexture);
	}
	break;

	case T_HPGAUGE: // Hp Gauge
	{
		UserInterfaceShader *UsingShader = new UserInterfaceShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 1, 2);
		SetTexture(UsingTexture);
	}
	break;

	case T_SIGNAL: // Signal - 몬스터가 플레이어를 인식했을 때 사용
	{
		EffectShader *UsingShader = new EffectShader();
		UsingShader->CreateShader(Device, RootSignature);
		SetShader(UsingShader);

		Texture *UsingTexture = new Texture();
		UsingTexture->CreateTexture(Device, CommandList, nullptr, Kind, 1, 2);
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

Shader* Material::m_LoadedShader = nullptr;
Shader* Material::m_SkinnedShader = nullptr;

void Material::PrepareShader(ID3D12Device* Device, ID3D12RootSignature* RootSignature)
{
	m_LoadedShader = new LoadedShader();
	m_LoadedShader->CreateShader(Device, RootSignature);

	m_SkinnedShader = new SkinnedShader();
	m_SkinnedShader->CreateShader(Device, RootSignature);
}

void Material::LoadMaterialInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File, GameObject* ParentFrame, int Type, Shader* InstanceShader)
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

			// 각 프레임에 맞는 Shader를 생성 및 설정
			Shader *UsingShader = nullptr;
			if (InstanceShader == nullptr) {
				switch (Type)
				{
				case 0:
				{
					UsingShader = m_LoadedShader;
					SetShader(UsingShader);
				}
				break;

				case 1:
				{
					UsingShader = m_SkinnedShader;
					SetShader(UsingShader);
				}
				break;
				}
			}
			else {
				switch (Type) {
				case 0:
				{
					UsingShader = new InstancingLoadedModel();
					UsingShader->CreateShader(Device, RootSignature);
					SetShader(UsingShader);
				}
				break;

				case 1:
				{
					UsingShader = new InstancingSkinnedModel();
					UsingShader->CreateShader(Device, RootSignature);
					SetShader(UsingShader);
				}
				break;
				}
			}
		}

		else if (!strcmp(Word, "<AlbedoColor>:")) {
			fread(&m_Diffuse, sizeof(float), 4, File);
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
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<SpecularMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<NormalMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<MetallicMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<EmissionMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<DetailAlbedoMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "<DetailNormalMap>:")) {
			LoadTextureInfo(Device, CommandList, File, ParentFrame);
		}

		else if (!strcmp(Word, "</Materials>")) {
			break;
		}
	}
}

void Material::LoadTextureInfo(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, FILE* File, GameObject* ParentFrame)
{
	char TextureName[64] = { '\0' };
	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(TextureName, sizeof(char), WordCount, File);
	TextureName[WordCount] = '\0';

	if (strcmp(TextureName, "null")) {
		// 텍스처의 이름이 null이 아니라면, 텍스처 파일의 이름을 결정 - 하위 폴더, dds 등 - (ex. Texture/~~~.dds)

		bool Duplicated = false;
		Duplicated = (TextureName[0] == '@'); // 텍스처의 첫번째 글자가 @이면 이미 가져왔으므로 중복 생성하지 않음

		if (Duplicated == false) {
			char FilePath[64] = { '\0' };
			strcpy_s(FilePath, 64, "Model/Texture/");
			strcat_s(FilePath, TextureName);
			strcat_s(FilePath, ".dds");

			size_t ConvertCount = 0;
			wchar_t* FrameTextureName = new wchar_t[64];
			mbstowcs_s(&ConvertCount, FrameTextureName, 64, FilePath, _TRUNCATE);

			// Texture를 생성
			Texture *UsingTexture = new Texture();
			UsingTexture->CreateTexture(Device, CommandList, FrameTextureName, -1, 1, 4);
			SetTexture(UsingTexture);

			delete[] FrameTextureName;
		}
		// 같은 텍스처를 사용하는 경우, 이미 생성된 텍스처를 찾아서 그것을 사용하도록 설정
		else {
			if (ParentFrame != nullptr) SetTexture(ParentFrame->FindDuplicatedTexture(TextureName));
		}
	}
}

char* Material::GetTextureName()
{
	if (m_Texture != nullptr) return m_Texture->GetTextureName();
	return nullptr;
}

void Material::SetPipeline(ID3D12GraphicsCommandList* CommandList)
{
	if (m_Shader != nullptr) m_Shader->Render(CommandList);
}

void Material::MappingTexture(ID3D12GraphicsCommandList* CommandList, int Index)
{
	CommandList->SetGraphicsRoot32BitConstants(1, 4, &m_Ambient, 16);
	CommandList->SetGraphicsRoot32BitConstants(1, 4, &m_Diffuse, 20);
	CommandList->SetGraphicsRoot32BitConstants(1, 4, &m_Specular, 24);
	CommandList->SetGraphicsRoot32BitConstants(1, 4, &m_Emissive, 28);

	CommandList->SetGraphicsRoot32BitConstants(1, 1, &m_Damaged, 32);

	if (m_Texture != nullptr) m_Texture->Render(CommandList, Index);
}