#include "c_menu.h"
#include "c_menu_ctrls.h"
#include "raw_menu_fonts.h"
#include "../../prot_wrapper.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "../../features/misc/sdk.h"
#include "../../variables/variables.hpp"

extern ImFont* smallest_pixel_7_12px;

static int tab = 0;
ImFont* menuFont, * tabFont, * controlFont, * boldMenuFont, * boldMenuFont_11px, * comboarrow_font;

void c_menu::init_fonts() {
	ImGuiIO& io = ImGui::GetIO();
	auto add_font = [&](const void* data, int size, float pixel_size) {
		return io.Fonts->AddFontFromMemoryTTF((void*)data, size, pixel_size, NULL, io.Fonts->GetGlyphRangesCyrillic());
		};

	menuFont = add_font(&verdana_raw, sizeof(verdana_raw), 12);
	tabFont = add_font(&menu_icons_raw, sizeof(menu_icons_raw), 42);
	comboarrow_font = add_font(&menu_icons_raw, sizeof(menu_icons_raw), 6);
	boldMenuFont = boldMenuFont_11px = controlFont = add_font(&verdana_bold_raw, sizeof(verdana_bold_raw), 12);
}

void c_menu::frame() {
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(6, 6);
	style->WindowRounding = 0.f;

	static bool first_frame = true;
	if (first_frame) {
		ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize * 0.5f - ImVec2(330, 170), ImGuiCond_FirstUseEver);
		first_frame = false;
	}

	ImGui::PushFont(menuFont);
	ImGui::SetNextWindowSize(ImVec2(660.f, 341.f));

	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;

	ImGui::BeginMenuBackground(xor ("##main"), &variables::menu::enabled, flags); {
		ImGui::BeginChild(xor ("Complete Border"), ImVec2(648.f, 329.f), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::EndChild();

		ImGui::SameLine(6.f);
		style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);

		ImGui::BeginChild(xor ("Menu Contents"), ImVec2(648.f, 331.f), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			style->ItemSpacing = ImVec2(0.f, -1.f);
			ImGui::ColorBar(xor ("penis ezoterika"), ImVec2(648.f, 2.f));

			ImGui::BeginTabs(xor ("Tabs"), ImVec2(75.f, 328.f), false,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				style->ItemSpacing = ImVec2(0.f, 0.f);
				style->ButtonTextAlign = ImVec2(0.5f, 0.47f);
				ImGui::PopFont();
				ImGui::PushFont(tabFont);

				const char* tab_icons[] = { "A", "B", "C", "D", "#" };
				ImGui::TabSpacer(xor ("##t-spacer"), ImVec2(75.f, 10.f));

				for (int i = 0; i < 4; i++) {
					if (tab == i) {
						if (ImGui::SelectedTab(tab_icons[i], ImVec2(75.f, 75.f))) tab = i;
					}
					else {
						if (ImGui::Tab(tab_icons[i], ImVec2(75.f, 75.f))) tab = i;
					}
				}
				ImGui::Tab(tab_icons[4], ImVec2(75.f, 75.f));

				ImGui::TabSpacer2(xor("##btm-spacer"), ImVec2(75.f, 7.f));
				ImGui::PopFont();
				ImGui::PushFont(menuFont);
				style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
			}
			ImGui::EndTabs();

			ImGui::SameLine(75.f);
			ImGui::BeginChild(xor ("Tab Contents"), ImVec2(572.f, 334.f), false,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				style->Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
				switch (tab) {
				case 0: aimbot(); break;
				case 1: visuals(); break;
				case 2: weapons(); break;
				case 3: settings(); break;
				}
				style->Colors[ImGuiCol_Border] = ImColor(10, 10, 10, 255);
			}
			ImGui::EndChild();

			style->ItemSpacing = ImVec2(4.f, 4.f);
			style->Colors[ImGuiCol_ChildBg] = ImColor(17, 17, 17, 255);
		}
		ImGui::EndChild();
		ImGui::PopFont();
	}
	ImGui::End();
}

void c_menu::aimbot() {
	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer(xor ("##sp1"));

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft(xor ("Vector Aimbot"), 173.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Enabled"), variables::aimbot::enabled);
			InsertKeybind(xor ("##vakey"), variables::aimbot::aimkey);
			InsertCombo(xor ("Hitbox"), variables::aimbot::bone, "Head\0Neck\0Body");
			InsertSlider(xor ("FOV"), variables::aimbot::fov, 0, 180, xor ("%.0f"));
			InsertCheckbox(xor ("Draw FOV"), variables::aimbot::drawfow);
			InsertColorPicker(xor ("##fovcol"), variables::colors::drawfovcol, true);
			InsertSlider(xor ("Smoothness"), variables::aimbot::smothness, 0, 100, (variables::aimbot::smothness <= 1) ? "Raw" : "%.0f%%");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##vabox1"), xor ("Vector Aimbot"));

		InsertSpacer(xor ("##sp2"));

		InsertGroupBoxLeft(xor ("Filters"), 76.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertSlider(xor ("Max Distance"), variables::aimbot::maxdist, 50, 300, xor ("%.0f m"));
			InsertCheckbox(xor ("Ignore Dead"), variables::aimbot::ignoredead);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##vabox2"), xor ("Filters"));
	}

	ImGui::NextColumn(); {
		InsertGroupBoxRight(xor ("Silent Aimbot"), 173.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Enabled"), variables::silent::enabled);
			InsertKeybind(xor ("##sakey"), variables::silent::aimkey);
			InsertCombo(xor ("Hitbox"), variables::silent::bone, "Head\0Neck\0Body");
			InsertSlider(xor ("FOV"), variables::silent::fov, 0, 180, xor ("%.0f"));
			InsertCheckbox(xor ("Draw FOV"), variables::silent::drawfow);
			InsertColorPicker(xor ("##safovcol"), variables::colors::drawfovsalocol, true);
			InsertSlider(xor ("Hitchance"), variables::silent::hitchance, 1, 100, xor ("%.0f%%"));

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##sabox1"), xor ("Silent Aimbot"));

		InsertSpacer(xor ("##sp3"));

		InsertGroupBoxRight(xor ("Other"), 48.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Magic Bullet"), variables::silent::magicbullet);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##sabox2"), xor ("Other"));
	}
}

void c_menu::visuals() {
	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer(xor ("##sp4"));

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft(xor ("Player ESP"), 171.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Skeleton "), variables::esp::bones);
			InsertColorPicker(xor ("##skelcol"), variables::colors::skelcol, false);

			InsertCheckbox(xor ("2D Box "), variables::esp::box);
			InsertColorPicker(xor ("##boxcol "), variables::colors::boxcol, false);
			InsertCheckbox(xor ("Fill Box "), variables::esp::fillbox);
			InsertColorPicker(xor ("##fillcol "), variables::colors::fillboxcol, true);
			InsertCheckbox(xor ("Weapon Text "), variables::esp::weapon_esp);
			InsertColorPicker(xor ("##wepcol "), variables::colors::curwepcol, false);
			InsertCheckbox(xor ("Distance "), variables::esp::distance_esp);
			InsertColorPicker(xor ("##distcol "), variables::colors::distancecol, false);
			InsertCheckbox(xor ("Health Bar "), variables::esp::hp);
			InsertColorPicker(xor ("##hpcol "), variables::colors::hpespcol, false);
			InsertComboWithoutText(xor ("##hppos "), variables::esp::hpbar_pos, "Left\0Right\0Bottom\0Top");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##espbox1"), xor ("Player ESP"));

		InsertSpacer(xor ("##sp123"));

		InsertGroupBoxLeft(xor ("Other"), 48.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Radar"), variables::visual_misc::radar);
			InsertColorPicker(xor ("##skelcol"), variables::colors::radar_markcol, false);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##espbox4"), xor ("Other"));
	}

	ImGui::NextColumn(); {
		InsertGroupBoxRight(xor ("Miscellaneous"), 106.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("2D Box Outline"), variables::esp::box_outline);
			InsertColorPicker(xor ("##boxoulinec"), variables::colors::box_outlinecol, false);
			InsertCheckbox(xor ("Health Bar Text"), variables::esp::hptext_bar);
			InsertColorPicker(xor ("##hptextc"), variables::colors::hp_textcol, false);

			InsertCheckbox(xor ("Skeleton Gradient"), variables::esp::skel_grd);
			InsertColorPicker(xor ("##skelgrd"), variables::colors::skel_grdcol, false);
			InsertCheckbox(xor ("Health Bar Gradient"), variables::esp::hp_grd);
			InsertColorPicker(xor ("##hpgrd"), variables::colors::hp_grdcol, false);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##espbox2"), xor ("Miscellaneous"));

		InsertSpacer(xor ("##sp0123"));

		InsertGroupBoxRight(xor ("Filters"), 111.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertSlider(xor ("Min Distance"), variables::esp::mindist, 0, 50, xor ("%.0f m"));
			InsertSlider(xor ("Max Distance"), variables::esp::maxdist, 50, 300, xor ("%.0f m"));
			InsertCheckbox(xor ("Ignore Dead"), variables::esp::ignoredead);
			//InsertCheckbox(xor ("Ignore Self"), variables::esp::ignoreself);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##espbox3"), xor ("Filters"));
	}
}

void c_menu::weapons() {
	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer(xor ("##sp6"));

	static float sethp_slider = 100.f;
	static float setarm_slider = 100.f;

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft(xor ("Health"), 181.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Godmode"), variables::player::godmode);
			InsertKeybind(xor ("##gmkey"), variables::player::gmkey);

			InsertButtonStart(("Set " + std::to_string((int)round(sethp_slider)) + " Health").c_str())
				SDK::PLAYER::Set_Health(sethp_slider + (SDK::PLAYER::Get_Max_Health() / 2));
			InsertButtonEnd;
			InsertSliderWithoutText(xor ("##shp"), sethp_slider, 0, 100, xor ("%.0fhp"));

			InsertButtonStart(("Set " + std::to_string((int)round(setarm_slider)) +" Armor").c_str())
				SDK::PLAYER::Set_Armor(setarm_slider);
			InsertButtonEnd;
			InsertSliderWithoutText(xor ("##sarm"), setarm_slider, 0, 100, xor ("%.0far"));

			InsertButtonStart(xor ("Suicide"))
				SDK::PLAYER::Set_Health(0);
			InsertButtonEnd;

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##wpbox1"), xor ("Health"));

		InsertSpacer(xor ("##sp  123"));

		InsertGroupBoxLeft(xor ("Player "), 66.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Disable Collision"), variables::player::nocollision);
			InsertCheckbox(xor ("Disable Ragdoll"), variables::player::noragdoll);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##box1233"), xor ("Player "));
	}

	ImGui::NextColumn(); {
		InsertGroupBoxRight(xor ("Features"), 86.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("NoClip"), variables::player::noclip);
			InsertKeybind(xor ("##nckey"), variables::player::noclipbind);
			InsertSliderWithoutText(xor ("##ncspeed"), variables::player::noclipspeed, 1, 25, xor ("%.0fm/s"));
			InsertCheckbox(xor ("Speed Boost"), variables::player::noclipboost);
			InsertKeybind(xor ("##boostkey"), variables::player::noclipboostbind);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##wpbox2"), xor ("Features"));

		InsertSpacer(xor ("##sp7"));

		InsertGroupBoxRight(xor ("Weapons"), 122.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("No Spread"), variables::weapon::nospread);
			InsertCheckbox(xor ("No Recoil"), variables::weapon::norecoil);
			InsertCheckbox(xor ("No Reload"), variables::weapon::noreload);
			InsertCheckbox(xor ("Max Range"), variables::weapon::range);
			InsertCheckbox(xor ("Max Speed"), variables::weapon::bulletintick);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##wpbox3"), xor ("Weapons"));
	}
}

std::string configs[128];
char cfgnamebuf[64];
int selected_config_id = 0;
int loaded_config_id = 0;

void c_menu::settings() {
	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer(xor ("##sp8"));

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft(xor ("Presets"), 289.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			ImVec2 cfgblock_pos = ImGui::GetCurrentWindow()->Pos + ImVec2(42, ImGui::GetCursorPosY());
			ImGui::SetNextWindowPos(cfgblock_pos);
			ImGui::SetNextWindowSizeConstraints(ImVec2(158.f, 124.f), ImVec2(FLT_MAX, 124.f));
			ImGui::BeginComboBackground(xor ("##cfgblock"), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoScrollbar);

			int configs_id = 0;
			if (!std::filesystem::exists(CFG_DIR_PATH)) {
				std::filesystem::create_directory(CFG_DIR_PATH);
			}

			for (const auto& entry : std::filesystem::directory_iterator(CFG_PATH)) {
				if (!std::filesystem::is_regular_file(entry.status())) continue;
				configs[++configs_id] = entry.path().filename().string();

				if (ImGui::SelectableConfig(configs[configs_id].c_str(), selected_config_id == configs_id, loaded_config_id == configs_id)) {
					selected_config_id = configs_id;
					strncpy(cfgnamebuf, configs[configs_id].c_str(), sizeof(cfgnamebuf) - 1);
				}
			}

			ImGui::EndCombo();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 130.f);
			ImGui::PushItemWidth(158.f);
			ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::InputText(xor ("##cfgname"), cfgnamebuf, sizeof(cfgnamebuf));
			ImGui::PopItemWidth();

			ImGui::Spacing();

			InsertButtonStart(xor ("Load"))
				cfg_mgmt::loadSettings(configs[selected_config_id]);
			loaded_config_id = selected_config_id;
			InsertButtonEnd;
			InsertButtonStart(xor ("Save"))
				cfg_mgmt::saveSettings(cfgnamebuf);
			loaded_config_id = selected_config_id;
			InsertButtonEnd;
			InsertButtonStart(xor ("Delete"))
				std::remove((CFG_PATH + configs[selected_config_id]).c_str());
			InsertButtonEnd;

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxLeft(xor ("##setbox1"), xor ("Presets"));
	}

	ImGui::NextColumn(); {
		InsertGroupBoxRight(xor ("Other"), 171.f); {
			ImGui::CustomSpacing(9.f);
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);

			InsertCheckbox(xor ("Watermark"), variables::visual_misc::watermark);

			const char* wm_fields[3] = { "Game", "FPS", "Time" };
			InsertMultiComboWithoutText(xor ("##wminfo"), wm_fields, variables::visual_misc::wm_elements, 3);

			InsertCheckbox(xor ("Crosshair"), variables::visual_misc::crosshair);
			InsertColorPicker(xor ("##crosscol"), variables::colors::crosshaircol, true);
			InsertComboWithoutText(xor ("##crosstype"), variables::visual_misc::crosshair_type, "Cross\0X-Cross\0Helicopter");
			if (variables::visual_misc::crosshair_type != 2) {
				InsertSliderWithoutText(xor ("##crosssize"), variables::visual_misc::crosshair_size, 4, 20, xor ("%.0fpx"));
			}
			else {
				InsertSliderWithoutText(xor ("##crosssize"), variables::visual_misc::nazi_cross_size, 8, 20, xor ("%.0fpx"));
			}

			InsertButtonStart(xor ("Unload"))
				variables::menu::unhook = true;
			InsertButtonEnd;

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight(xor ("##setbox2"), xor ("Other"));
	}
}

void c_menu::cpicker(const char* name, float* color, bool alpha) {
	ImGui::SameLine(219.f);
	ImGui::ColorEdit4((std::string{ xor ("##cp") } + name).c_str(), color,
		(alpha ? ImGuiColorEditFlags_AlphaBar : ImGuiColorEditFlags_NoAlpha) |
		ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
}

bool c_menu::keybind(const char* label, int* key, bool show_label) {
	str_crypt_start;

	static const char* keys[] = {
"-","M1","M2","CN","M3","M4","M5","-","BCK","TAB","-","-","CLR","ENT","-","-","SHF","CTRL","ALT","PAU","CAP","KAN","-","JUN","FIN","KAN","-","ESC","CON",
"NCO","ACC","MAD","SPC","PGU","PGD","END","HOM","LEF","UP","RIG","DOW","SEL","PRI","EXE","printscrin","INS","DEL","HEL","0","1","2","3","4","5","6","7","8","9","-","-","-","-","-","-","-","A",
"B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","Win","Win","APP","-","SLE","Num 0","Num 1","Num 2","Num 3","Num 4","Num 5",
"Num 6","Num 7","Num 8","Num 9","MUL","ADD","SEP","MIN","DEL","DIV","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20",
"F21","F22","F23","F24","-","-","-","-","-","-","-","-","NUM","SCR","EQU","MAS","TOY","OYA","OYA","-","-","-","-","-","-","-","-","-","SHF","SHF","CTR","CTR","Alt","Alt"
	};

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(label);
	const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(27.f, 14.f));

	ImGui::ItemSize(ImRect(rect.Min, rect.Max + ImVec2(0, 2)));
	if (!ImGui::ItemAdd(rect, id)) return false;

	char buf_display[64] = "[-]";
	bool value_changed = false;
	int k = *key;

	if (*key != 0 && g.ActiveId != id) {
		snprintf(buf_display, sizeof(buf_display), xor ("[%s]"), keys[*key]);
	}
	else if (g.ActiveId == id) {
		snprintf(buf_display, sizeof(buf_display), xor ("[%s]"), keys[*key]);
	}

	const ImVec2 label_size = smallest_pixel_7_12px->CalcTextSizeA(12.f, FLT_MAX, 0, buf_display);
	ImRect clickable(ImVec2(rect.Max.x - 8 - label_size.x, rect.Min.y), rect.Max);
	bool hovered = ImGui::ItemHoverable(clickable, id);

	ImGui::PushClipRect(clickable.Min, clickable.Max, true);
	for (int i = 0; i < 4; i++) {
		window->DrawList->AddText(smallest_pixel_7_12px, 12.f,
			clickable.Min + ImVec2(3 + i % 2, 1 + i / 2), ImColor(0, 0, 0, 255), buf_display);
	}
	window->DrawList->AddText(smallest_pixel_7_12px, 12.f, clickable.Min + ImVec2(4, 2),
		ImGui::IsItemActive() ? ImColor(255, 0, 0, 255) : ImColor(114, 114, 114, 255), buf_display);
	ImGui::PopClipRect();

	if (hovered && ImGui::GetIO().MouseClicked[0]) {
		if (g.ActiveId != id) {
			memset(ImGui::GetIO().MouseDown, 0, sizeof(ImGui::GetIO().MouseDown));
			memset(ImGui::GetIO().KeysDown, 0, sizeof(ImGui::GetIO().KeysDown));
			*key = 0;
		}
		ImGui::SetActiveID(id, window);
	}
	else if (ImGui::GetIO().MouseClicked[0] && g.ActiveId == id) {
		ImGui::ClearActiveID();
	}

	if (g.ActiveId == id) {
		for (int i = 0; i < 5; i++) {
			if (ImGui::GetIO().MouseDown[i]) {
				*key = i == 4 ? 0x06 : (1 << i);
				value_changed = true;
				ImGui::ClearActiveID();
				break;
			}
		}

		if (!value_changed) {
			for (int i = 0x08; i <= 0xA5; i++) {
				if (ImGui::GetIO().KeysDown[i]) {
					*key = i;
					value_changed = true;
					ImGui::ClearActiveID();
					break;
				}
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			*key = 0;
			ImGui::ClearActiveID();
		}
	}

	if (hovered || ImGui::IsItemActive()) {
		ImGui::SetMouseCursor(hovered ? ImGuiMouseCursor_Hand : ImGuiMouseCursor_TextInput);
	}

	str_crypt_end;
	return value_changed;
}

c_menu menya_pizdili_v_detstve;