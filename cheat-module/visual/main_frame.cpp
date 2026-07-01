#include <windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "../variables/variables.hpp"

#include "../features/esp/esp.h"
#include "visual_misc.h"
#include "../features/misc/functions_init.h"
#include "../game/pools/game_pools.hpp"

#include "../mne_pohuy/skeet/c_menu.h"
extern c_menu menya_pizdili_v_detstve;
extern ImFont* boldMenuFont_11px;

namespace overlay {
	void draw() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0.f, 0.f });
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
		const ImGuiWindowFlags flags = 
			ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoBackground;

		ImGui::Begin("##overl4y", NULL, flags);
		{
			esp::tick();

			ImGui::PushFont(boldMenuFont_11px); {
				visual_misc::init_all();
			} ImGui::PopFont();

			if (variables::menu::enabled) {
				menya_pizdili_v_detstve.frame();
			}
			_hacks::tick();
		}

		ImGui::End();
		ImGui::EndFrame();
	}
}