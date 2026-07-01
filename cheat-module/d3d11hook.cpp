
// from peroxide

#include "d3d11hook.hpp"

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>

#include "minhook/include/MinHook.h"
#pragma comment(lib, "libMinHook.x64.lib")

#include "variables/variables.hpp"
#include "visual/main_frame.h"
#include "mne_pohuy/skeet/c_menu.h"
#include "features/esp/esp.h"
#include "game/pools/game_pools.hpp"

#include "prot_wrapper.h"

typedef HRESULT(__stdcall* d3d11_present_hk) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* d3d11_resizebuffers_hk)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern c_menu menya_pizdili_v_detstve;

namespace d3d11hook {
	d3d11_present_hk			_present_hk;
	d3d11_resizebuffers_hk		_resizebuffers_hk;
	HWND						_hwnd = 0;
	WNDCLASSEXA					_wnd_class;
	WNDPROC						_wnd_proc;
	IDXGISwapChain*				dx_swapchain;
	ID3D11Device*				dx_device = nullptr;
	ID3D11DeviceContext*		dx_ctx = nullptr;
	ID3D11RenderTargetView*		dx_rendertargetview = nullptr;
	DWORD_PTR					*_swapchain_vt = 0, *_device_vt = 0, *_ctx_vt = 0;
	std::once_flag				_render_init;

	void init_imgui()
	{
		ImGui::CreateContext();
		{
			ImGuiIO& io = ImGui::GetIO();
			//io.MouseDrawCursor = true;
			//io.WantCaptureMouse = true;
			//io.WantCaptureKeyboard = true;
			//io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			io.IniFilename = NULL;
		}

		menya_pizdili_v_detstve.init_fonts();
		esp::init_fonts();

		ImGui_ImplWin32_Init(_hwnd);
		ImGui_ImplDX11_Init(dx_device, dx_ctx);
	}

	LRESULT __stdcall _wnd_proc_hk(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_KEYDOWN:
			if (wParam == VK_DELETE) {
				ImGuiIO& io = ImGui::GetIO();
				variables::menu::enabled = !variables::menu::enabled;

				io.WantCaptureMouse = variables::menu::enabled;
				io.WantCaptureKeyboard = variables::menu::enabled;

				if (variables::menu::enabled) {
					io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
					io.MouseDrawCursor = true;
				}
				else {
					io.MouseDrawCursor = false;
					io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
				}

				break;
			}

			if (variables::player::noclip && !variables::menu::enabled) {
				if (wParam == variables::player::noclipbind) {
					variables::player::noclip_state = !variables::player::noclip_state;
				}
			}

			if (wParam == variables::player::gmkey) {
				variables::player::godmode = !variables::player::godmode;
			}

		}

		if (variables::menu::enabled && !variables::menu::unhook) {
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
			return true;
		}

		return CallWindowProc(_wnd_proc, hWnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall detour_present_hk(IDXGISwapChain* _dx_swapchain, UINT _sync_intrv, UINT _flags)
	{
		std::call_once(_render_init, [&]() {
			dx_swapchain = _dx_swapchain;
			HRESULT hr = _dx_swapchain->GetDevice(__uuidof(ID3D11Device), (void**)&dx_device);
			if (SUCCEEDED(hr))
			{
				dx_device->GetImmediateContext(&dx_ctx);
				DXGI_SWAP_CHAIN_DESC sd;
				_dx_swapchain->GetDesc(&sd);
				_hwnd = sd.OutputWindow;
				ID3D11Texture2D* _backbuffer;
				_dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_backbuffer);
				dx_device->CreateRenderTargetView(_backbuffer, 0, &dx_rendertargetview);
				_backbuffer->Release();
				_wnd_proc = (WNDPROC)SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR)_wnd_proc_hk);

				init_imgui();
			}
			else return _present_hk(_dx_swapchain, _sync_intrv, _flags);
			});

		if (variables::menu::unhook) return _present_hk(_dx_swapchain, _sync_intrv, _flags);

		g_pools.update_players();
		overlay::draw();
		ImGui::Render();


		dx_ctx->OMSetRenderTargets(1, &dx_rendertargetview, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return _present_hk(_dx_swapchain, _sync_intrv, _flags);
	}

	LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void cleanup_render_target() {
		if (dx_rendertargetview) {
			dx_rendertargetview->Release();
			dx_rendertargetview = nullptr;
		}
	}

	void create_render_target() {
		ID3D11Texture2D* pBackBuffer = nullptr;
		if (SUCCEEDED(dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) {
			dx_device->CreateRenderTargetView(pBackBuffer, NULL, &dx_rendertargetview);
			pBackBuffer->Release();
		}
	}

	HRESULT __stdcall detour_resizebuffers_hk(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
		cleanup_render_target();
		HRESULT hr = _resizebuffers_hk(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		if (SUCCEEDED(hr)) {
			dx_swapchain = pSwapChain;

			if (!dx_device) {
				pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&dx_device);
				dx_device->GetImmediateContext(&dx_ctx);
			}
			create_render_target();
		}

		return hr;
	}


	void init() {

		MH_Initialize();

		// if altv ... maybe
		//_hwnd = FindWindowA("grcWindow", 0);

		std::string random_class_name;
		int random_num = 1000 + rand() % 9000;
		random_class_name = std::to_string(random_num);
		//random_class_name = "000";

		_wnd_class = {
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			DXGIMsgProc,
			0L, 0L,
			LI_CALL(GetModuleHandleA)(0),
			0, 0, 0, 0,
			random_class_name.c_str(),
			0 
		};
		LI_CALL(RegisterClassExA)(&_wnd_class);

		_hwnd = LI_CALL(CreateWindowExA)(
			WS_EX_TRANSPARENT, // 0L - def
			random_class_name.c_str(),
			0,
			WS_OVERLAPPEDWINDOW | WS_DISABLED, // WS_POPUP
			0, 0, 1, 1,
			0, 0, _wnd_class.hInstance,
			0
		);

		LI_CALL(SetLayeredWindowAttributes)(_hwnd, 0, 0, LWA_ALPHA);
		LI_CALL(ShowWindow)(_hwnd, SW_HIDE);
		//LI_CALL(ShowWindow)(FindWindowA("grcWindow", 0), SW_SHOWNORMAL);

		D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
		D3D_FEATURE_LEVEL obtainedLevel;
		DXGI_SWAP_CHAIN_DESC sd;
		{
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 1;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			sd.OutputWindow = _hwnd;
			sd.SampleDesc.Count = 1;
			sd.Windowed = true; // ((GetWindowLongPtr(_hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			sd.BufferDesc.Width = 1;
			sd.BufferDesc.Height = 1;
			sd.BufferDesc.RefreshRate.Numerator = 0;
			sd.BufferDesc.RefreshRate.Denominator = 1;
		}

		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION,
			&sd, &dx_swapchain, &dx_device, &obtainedLevel, &dx_ctx);
		if (FAILED(hr)) {
			//_log("D3D11CreateDeviceAndSwapChain err");
			return;
		}

		_swapchain_vt = (DWORD_PTR*)dx_swapchain;
		_swapchain_vt = (DWORD_PTR*)_swapchain_vt[0];

		_ctx_vt = (DWORD_PTR*)dx_ctx;
		_ctx_vt = (DWORD_PTR*)_ctx_vt[0];

		_device_vt = (DWORD_PTR*)dx_device;
		_device_vt = (DWORD_PTR*)_device_vt[0];

		// present
		if (MH_CreateHook((DWORD_PTR*)_swapchain_vt[8], detour_present_hk, reinterpret_cast<void**>(&_present_hk)) != MH_OK)
			return;
		if (MH_EnableHook((DWORD_PTR*)_swapchain_vt[8]) != MH_OK)
			return;

		// rbh
		if (MH_CreateHook((DWORD_PTR*)_swapchain_vt[13], detour_resizebuffers_hk, reinterpret_cast<void**>(&_resizebuffers_hk)) != MH_OK)
			return;
		if (MH_EnableHook((DWORD_PTR*)_swapchain_vt[13]) != MH_OK)
			return;

		DWORD dwOld;
		LI_CALL(VirtualProtect)(_present_hk, 2, PAGE_EXECUTE_READWRITE, &dwOld);

		return;
	}

	void release() {
		SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR)_wnd_proc);

		cleanup_render_target();

		dx_swapchain->Release();
		dx_swapchain = NULL;

		dx_device->Release();
		dx_device = NULL;

		dx_ctx->Release();
		dx_ctx = NULL;

		LI_CALL(DestroyWindow)(_hwnd);
		LI_CALL(UnregisterClassA)(_wnd_class.lpszClassName, _wnd_class.hInstance);

		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}