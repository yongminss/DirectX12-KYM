#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();
}

void Scene::CreateRootSignature(ID3D12Device &Device)
{
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;

}