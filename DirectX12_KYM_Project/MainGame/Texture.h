#pragma once
// 텍스처 매핑에 사용할 Buffer를 생성 및 관리
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