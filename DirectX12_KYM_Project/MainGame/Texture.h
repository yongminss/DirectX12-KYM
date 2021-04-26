#pragma once
// �ؽ�ó ���ο� ����� Buffer�� ���� �� ����
class Texture
{
private:
	ID3D12Resource *m_TextureBuffer = nullptr;
	ID3D12Resource *m_UploadTextureBuffer = nullptr;

public:
	Texture();
	~Texture();

	void CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind);
};