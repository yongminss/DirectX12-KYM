// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <iostream>
#include <vector>
#include <chrono>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

// Window Size
#define Window_Width 800
#define Window_Height 600

// HeightMap Size ( x : 0 - 5100, z : 0 - 5100)
#define MAP_SCALE 20
#define MAP_SIZE 5000

static SIZE_T DescriptorHandleIncrementSize = 0;

// Animation Type
#define ANIMATION_TYPE_LOOP 0
#define ANIMATION_TYPE_ONCE 1
#define ANIMATION_TYPE_CONVERT 2

// Texture Kinds
enum TEXTURE_KIND
{
	// 0 ~ 4 - UserInterface
	T_TERRAIN,
	T_SKYBOX,
	T_HPBAR,
	T_HPGAUGE,
	T_AIM,
	// 5 ~ 6 - Billboard Texture
	T_GRASS,
	T_TREE,
	// 7 ~ - In Game Effect
	T_SIGNAL
};

// Monster Kinds
enum MONSTER_KIND
{
	M_WEAKORC,
	M_STRONGORC,
	M_SHAMANORC,
	M_WOLFRIDERORC
};

// Player Animation Kinds
enum P_ANIMATION
{
	P_IDLE,

	// 1 ~ 8 - Run Animation
	P_RUN_FRONT,
	P_RUN_BACK,
	P_RUN_RIGHT,
	P_RUN_LEFT,
	P_RUN_FRONT_RIGHT,
	P_RUN_FRONT_LEFT,
	P_RUN_BACK_RIGHT,
	P_RUN_BACK_LEFT,

	// 9 ~ 17 - Shoot Animation
	P_SHOOT,
	P_RUN_SHOOT_FRONT,
	P_RUN_SHOOT_BACK,
	P_RUN_SHOOT_RIGHT,
	P_RUN_SHOOT_LEFT,
	P_RUN_SHOOT_FRONT_RIGHT,
	P_RUN_SHOOT_FRONT_LEFT,
	P_RUN_SHOOT_BACK_RIGHT,
	P_RUN_SHOOT_BACK_LEFT,

	// 18 ~ 26 - Reload Animation
	P_RELOAD,
	P_RUN_RELOAD_FRONT,
	P_RUN_RELOAD_BACK,
	P_RUN_RELOAD_RIGHT,
	P_RUN_RELOAD_LEFT,
	P_RUN_RELOAD_FRONT_RIGHT,
	P_RUN_RELOAD_FRONT_LEFT,
	P_RUN_RELOAD_BACK_RIGHT,
	P_RUN_RELOAD_BACK_LEFT,

	P_ROLL,
	P_DAMAGED,
	P_DEATH
};

// Monster Animation Kinds
enum M_ANIMATION
{
	M_IDLE,
	M_WALK,
	M_RUN,
	M_ATTACK_A,
	M_ATTACK_B,
	M_DAMAGED,
	M_DEATH_A,
	M_DEATH_B
};

extern ID3D12Resource* CreateBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* VertexData, int Size, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES ResourceState, ID3D12Resource *UploadBuffer);