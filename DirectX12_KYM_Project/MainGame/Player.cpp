#include "stdafx.h"
#include "Player.h"


Player::Player()
{

}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

void Player::CreatePlayer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	m_Mesh = new Mesh();
	m_Mesh->CreateMesh(Device, CommandList);

	m_Shader = new Shader();
	m_Shader->CreateShader(Device, RootSignature);

	// Player 오브젝트가 바라보는 화면을 플레이어에게 보여줄 수 있게 Camera 생성
	m_Camera = new Camera();
	m_Camera->CreateCamera();
}

void Player::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, this);

	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	if (m_Mesh != nullptr) m_Mesh->Render(CommandList);
}