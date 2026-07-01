#pragma once

// imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"

#include <Windows.h>
#include <vector>
#include <d3d11.h>
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#pragma warning(disable : 4244)

namespace artemfatality2008 {
	void init_base(HMODULE hmod);
	DWORD WINAPI main_thread(HMODULE hmod);
}


#include <DirectXMath.h>
using namespace DirectX;

#ifdef DEBUG
#define log_smth(log) std::cout << log << std::endl;
#else
#define log_smth
#endif