#pragma once
// DirectX Device �ʱ�ȭ �� ����
class GameFramework
{
	IDXGIFactory4 *m_Factory = nullptr;
	ID3D12Device *m_Device = nullptr;

public:
	GameFramework();
	~GameFramework();

	void CreateGameFramework();
	void CreateDirectDevice();
};
