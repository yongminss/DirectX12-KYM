#pragma once
// DirectX Device �ʱ�ȭ �� ����
class GameFramework
{
	IDXGIFactory4 *m_Factory = nullptr;

public:
	GameFramework();
	~GameFramework();

	void CreateGameFramework();
	void CreateDirectDevice();
};
