#pragma once
// �ؽ�ó ���ο� ����� Buffer�� ���� �� ����
class Texture
{
private:
	ID3D12Resource **m_TextureBuffer = nullptr;
	ID3D12Resource **m_UploadTextureBuffer = nullptr;

	ID3D12DescriptorHeap *m_ShaderResourceViewDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorHandle{};

	char *m_TextureName = nullptr;

	int m_RootParameterIndex = 0;
	int m_TextureCount = 0;
	SIZE_T m_DescriptorHandleIncrementSize = 0;

public:
	Texture();
	~Texture();

	void CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, wchar_t* TextureName, int Kind, int TextureCount, int RootParameterIndex);
	void CreateTextureBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, wchar_t* TextureName, int Kind);
	void CreateDescriptorHeap(ID3D12Device* Device);
	void CreateShaderResourceView(ID3D12Device* Device);

	char* GetTextureName() { return m_TextureName; }

	void Render(ID3D12GraphicsCommandList* CommandList, int TextureIndex);
};