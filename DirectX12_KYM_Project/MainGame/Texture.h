#pragma once
// 텍스처 매핑에 사용할 Buffer를 생성 및 관리
class Texture
{
private:
	ID3D12Resource *m_TextureBuffer = nullptr;
	ID3D12Resource *m_UploadTextureBuffer = nullptr;

	ID3D12DescriptorHeap *m_ShaderResourceViewDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorHandle{};

public:
	Texture();
	~Texture();

	void CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind);
	void CreateTextureBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind);
	void CreateDescriptorHeap(ID3D12Device* Device);
	void CreateShaderResourceView(ID3D12Device* Device);

	void Render(ID3D12GraphicsCommandList* CommandList);
};