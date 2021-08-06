#include "stdafx.h"
#include "Material.h"


Material::Material()
{

}

Material::~Material()
{

}

void Material::LoadMaterialInfo(FILE *File)
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
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<SpecularMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<NormalMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<MetallicMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<EmissionMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<DetailAlbedoMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "<DetailNormalMap>:")) {
			LoadTextureInfo(File);
		}

		else if (!strcmp(Word, "</Materials>")) {
			break;
		}
	}
}

void Material::LoadTextureInfo(FILE *File)
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
			// Texture�� ����
		}
	}
}