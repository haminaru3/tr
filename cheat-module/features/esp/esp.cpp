#include "ESP.h"
#include <windows.h>
#include <unordered_map>

#include "imgui/imgui.h"
#include "../../memory/math.h"
#include "../misc/sdk.h"
#include "../../variables/variables.hpp"
#include "../../game/pools/game_pools.hpp"

#include <string>
#include <cstdlib>
#include "imgui/imgui_internal.h"
#include "imgui/imgui_freetype.h"
#include <sstream>

#include "../../mne_pohuy/fonts_data.h"
#include "../../prot_wrapper.h"

#define is_esp_generally_enabled variables::esp::bones || variables::esp::box || variables::esp::fillbox || variables::esp::weapon_esp || variables::esp::distance_esp || variables::esp::hp

ImFont* smallest_pixel_7 = nullptr;
ImFont* smallest_pixel_7_12px = nullptr;

namespace esp
{
	void init_fonts() {
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		smallest_pixel_7 = io.Fonts->AddFontFromMemoryTTF(&smallestpixel7_raw, sizeof(smallestpixel7_raw), 14, NULL, io.Fonts->GetGlyphRangesCyrillic());
		smallest_pixel_7_12px = io.Fonts->AddFontFromMemoryTTF(&smallestpixel7_raw, sizeof(smallestpixel7_raw), 12, NULL, io.Fonts->GetGlyphRangesCyrillic());
	}

	void outlinedtext(float size, std::string label, ImVec2 pedpos, float maincol[4]) {
		auto draw_list = ImGui::GetBackgroundDrawList();

		float y = pedpos.y;
		ImVec2 textSize = smallest_pixel_7->CalcTextSizeA(size, FLT_MAX, 0, label.c_str());

		if (variables::esp::hpbar_pos == 2 && variables::esp::hp) y += 10.f;
		else if (size != 10) y += 4.f;

		ImVec2 textpos(pedpos.x - (textSize.x / 2), y);

		draw_list->AddText(smallest_pixel_7, size, ImVec2(textpos.x - 1, textpos.y - 1), ImColor(0, 0, 0, 255), label.c_str());
		draw_list->AddText(smallest_pixel_7, size, ImVec2(textpos.x + 1, textpos.y + 1), ImColor(0, 0, 0, 255), label.c_str());
		draw_list->AddText(smallest_pixel_7, size, ImVec2(textpos.x - 1, textpos.y + 1), ImColor(0, 0, 0, 255), label.c_str());
		draw_list->AddText(smallest_pixel_7, size, ImVec2(textpos.x + 1, textpos.y - 1), ImColor(0, 0, 0, 255), label.c_str());

		int vert_start_idx = draw_list->VtxBuffer.Size;
		draw_list->AddText(smallest_pixel_7, size, textpos, ImColor(maincol[0], maincol[1], maincol[2], maincol[3]), label.c_str());
		ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, draw_list->VtxBuffer.Size, textpos, textpos + ImVec2(0, textSize.y),
			ImColor(maincol[0], maincol[1], maincol[2], maincol[3]),
			ImColor(maincol[0] / 1.5f, maincol[1] / 1.5f, maincol[2] / 1.5f, maincol[3]));
	}

	void draw_bone_line(ImVec2 start, ImVec2 stop)
	{
		auto draw_list = ImGui::GetBackgroundDrawList();
		draw_list->AddLine(start, stop, ImColor(variables::colors::skelcol[0], variables::colors::skelcol[1], variables::colors::skelcol[2], variables::colors::skelcol[3]), 1.f);
	}

	void draw_bones(s_entity_bones bones) {
		Vector3 bones_coords[][2] = {
			{ bones.NECK, bones.RIGHT_CLAVICLE },
			{ bones.RIGHT_CLAVICLE, bones.RIGHT_UPPER_ARM },
			{ bones.RIGHT_UPPER_ARM, bones.RIGHT_FOREARM },
			{ bones.RIGHT_FOREARM, bones.RIGHT_HAND },

			{ bones.NECK, bones.LEFT_CLAVICLE },
			{ bones.LEFT_CLAVICLE, bones.LEFT_UPPER_ARM },
			{ bones.LEFT_UPPER_ARM, bones.LEFT_FOREARM },
			{ bones.LEFT_FOREARM, bones.LEFT_HAND },

			{ bones.NECK, bones.SPINE3 },
			{ bones.SPINE3, bones.SPINE2 },
			{ bones.SPINE2, bones.SPINE1 },
			{ bones.SPINE1, bones.SPINE_ROOT },

			{ bones.SPINE_ROOT, bones.RIGHT_THIGH },
			{ bones.RIGHT_THIGH ,bones.RIGHT_CALF },
			{ bones.RIGHT_CALF,bones.RIGHT_FOOT },

			{ bones.SPINE_ROOT, bones.LEFT_THIGH },
			{ bones.LEFT_THIGH,bones.LEFT_CALF },
			{ bones.LEFT_CALF,bones.LEFT_FOOT },
		};

		int size_mainskel = sizeof(bones_coords) / sizeof(bones_coords[0]);

		for (size_t i = 0; i < size_mainskel; i++) {
			//ImVec2 start_pos = SDK::WorldToScreen(bones_coords[i][0]);
			//ImVec2 end_pos = SDK::WorldToScreen(bones_coords[i][1]);

			ImVec2 start_pos;
			ImVec2 end_pos;
			if (!SDK::WorldToScreen(bones_coords[i][0], &start_pos)) return;
			if (!SDK::WorldToScreen(bones_coords[i][1], &end_pos)) return;

			draw_bone_line(start_pos, end_pos);
		}
	}

	enum healthbar_pos {
		LEFT,
		RIGHT,
		BOTTOM,
		TOP
	};

	void draw_hp_bar(ImDrawList* draw_list, ImVec2 top_pos, ImVec2 btm_pos, float health_percent, float arm_percent, healthbar_pos pos_var) {
		ImVec2 healthbar_p1, healthbar_p2;
		ImVec2 healthbar_bg_p1, healthbar_bg_p2;
		ImVec2 armbar_p1, armbar_p2;
		ImVec2 armbar_bg_p1, armbar_bg_p2;
		ImVec2 grd_p1, grd_p2;

		float width = (btm_pos.y - top_pos.y) / 3.5f;
		float height = top_pos.y - btm_pos.y;
		float width_percent = (width / 100) * 2;
		float height_percent = height / 100;

		ImColor hpbarcolor(variables::colors::hpespcol[0], variables::colors::hpespcol[1], variables::colors::hpespcol[2], variables::colors::hpespcol[3]);
		ImColor hpbarcolor_grd(variables::colors::hp_grdcol[0], variables::colors::hp_grdcol[1], variables::colors::hp_grdcol[2], variables::colors::hp_grdcol[3]);
		ImColor armbarcolor(variables::colors::hp_arm_espcol[0], variables::colors::hp_arm_espcol[1], variables::colors::hp_arm_espcol[2], variables::colors::hp_arm_espcol[3]);

		switch (pos_var) {
		case LEFT:
			healthbar_bg_p1 = ImVec2(btm_pos.x - width - 6.0f, btm_pos.y + 2);
			healthbar_bg_p2 = ImVec2(btm_pos.x - width - 6.0f, top_pos.y - 2);
			healthbar_p1 = ImVec2(btm_pos.x - width - 6.0f, btm_pos.y + 1);
			healthbar_p2 = ImVec2(btm_pos.x - width - 6.0f, (top_pos.y - height - 1) + height_percent * health_percent);

			armbar_bg_p1 = ImVec2(btm_pos.x - width - 12.0f, btm_pos.y + 2);
			armbar_bg_p2 = ImVec2(btm_pos.x - width - 12.0f, top_pos.y - 2);
			armbar_p1 = ImVec2(btm_pos.x - width - 12.0f, btm_pos.y + 1);
			armbar_p2 = ImVec2(btm_pos.x - width - 12.0f, (top_pos.y - height - 1) + height_percent * arm_percent);

			grd_p1 = ImVec2(btm_pos.x - width - 6, top_pos.y - (height / 2));
			grd_p2 = ImVec2(btm_pos.x - width - 5, top_pos.y - (height / 2));
			break;

		case RIGHT:
			healthbar_bg_p1 = ImVec2(btm_pos.x + width + 6.0f, btm_pos.y + 2);
			healthbar_bg_p2 = ImVec2(btm_pos.x + width + 6.0f, top_pos.y - 2);
			healthbar_p1 = ImVec2(btm_pos.x + width + 6.0f, btm_pos.y + 1);
			healthbar_p2 = ImVec2(btm_pos.x + width + 6.0f, (top_pos.y - height - 1) + height_percent * health_percent);

			armbar_bg_p1 = ImVec2(btm_pos.x + width + 12.0f, btm_pos.y + 2);
			armbar_bg_p2 = ImVec2(btm_pos.x + width + 12.0f, top_pos.y - 2);
			armbar_p1 = ImVec2(btm_pos.x + width + 12.0f, btm_pos.y + 1);
			armbar_p2 = ImVec2(btm_pos.x + width + 12.0f, (top_pos.y - height - 1) + height_percent * arm_percent);

			grd_p1 = ImVec2(btm_pos.x + width + 12, top_pos.y - (height / 2));
			grd_p2 = ImVec2(btm_pos.x + width + 13, top_pos.y - (height / 2));
			break;

		case BOTTOM:
			healthbar_bg_p1 = ImVec2(btm_pos.x - width - 2, btm_pos.y + 6);
			healthbar_bg_p2 = ImVec2(btm_pos.x + width + 2, btm_pos.y + 6);
			healthbar_p1 = ImVec2(btm_pos.x - width - 1, btm_pos.y + 6);
			healthbar_p2 = ImVec2((btm_pos.x - width + 1) + width_percent * health_percent, btm_pos.y + 6);

			armbar_bg_p1 = ImVec2(btm_pos.x - width - 2, btm_pos.y + 12);
			armbar_bg_p2 = ImVec2(btm_pos.x + width + 2, btm_pos.y + 12);
			armbar_p1 = ImVec2(btm_pos.x - width - 1, btm_pos.y + 12);
			armbar_p2 = ImVec2((btm_pos.x - width + 1) + width_percent * arm_percent, btm_pos.y + 12);

			grd_p1 = ImVec2(btm_pos.x + (width / 2), btm_pos.y + 6);
			grd_p2 = ImVec2(btm_pos.x + (width / 2), btm_pos.y + 7);
			break;

		case TOP:
			healthbar_bg_p1 = ImVec2(top_pos.x - width - 2, top_pos.y - 6);
			healthbar_bg_p2 = ImVec2(top_pos.x + width + 2, top_pos.y - 6);
			healthbar_p1 = ImVec2(top_pos.x - width - 1, top_pos.y - 6);
			healthbar_p2 = ImVec2((top_pos.x - width + 1) + width_percent * health_percent, top_pos.y - 6);

			armbar_bg_p1 = ImVec2(top_pos.x - width - 2, top_pos.y - 12);
			armbar_bg_p2 = ImVec2(top_pos.x + width + 2, top_pos.y - 12);
			armbar_p1 = ImVec2(top_pos.x - width - 1, top_pos.y - 12);
			armbar_p2 = ImVec2((top_pos.x - width + 1) + width_percent * arm_percent, top_pos.y - 12);

			grd_p1 = ImVec2(top_pos.x + (width / 2), top_pos.y - 6);
			grd_p2 = ImVec2(top_pos.x + (width / 2), top_pos.y - 7);
			break;
		}

		draw_list->AddLine(healthbar_bg_p1, healthbar_bg_p2, ImColor(0, 0, 0, 255), 4.f);
		if (variables::esp::hp_grd) {
			int vert_start_idx = draw_list->VtxBuffer.Size;
			draw_list->AddLine(healthbar_p1, healthbar_p2, hpbarcolor, 2.f);
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, draw_list->VtxBuffer.Size, healthbar_p1, healthbar_p2,
				hpbarcolor, hpbarcolor_grd);
		}
		else {
			draw_list->AddLine(healthbar_p1, healthbar_p2, hpbarcolor, 2.f);
		}

		if (arm_percent > 1) {
			draw_list->AddLine(armbar_bg_p1, armbar_bg_p2, ImColor(0, 0, 0, 255), 4.f);
			draw_list->AddLine(armbar_p1, armbar_p2, armbarcolor, 2.f);
		}
	}


	void tick()
	{
		if (is_esp_generally_enabled) {

			std::lock_guard<std::mutex> lock(g_pools.players_mutex);

			for (auto& player : g_pools.players) {
				if (!_is_valid_ptr(player.ped)) continue;
				if (player.ped == 0) continue;

				if (!SDK::LocalPlayer)
					return;

				if (player.ped == SDK::LocalPlayer) continue;

				float health = player.health;
				if (variables::esp::ignoredead && health <= 0.f) continue;

				if (!(player.distance >= variables::esp::mindist && player.distance <= variables::esp::maxdist)) continue;

				Vector3 ped_pos = Vector3(player.position.x, player.position.y, player.position.z);

				Vector3 w_top, w_bottom;
				w_top = Vector3(ped_pos.x, ped_pos.y, ped_pos.z + 0.89f);
				w_bottom = Vector3(ped_pos.x, ped_pos.y, ped_pos.z - 1.0f);

				ImVec2 s_top, s_bottom;
				if (!SDK::WorldToScreen(w_top, &s_top) || !SDK::WorldToScreen(w_bottom, &s_bottom)) continue;

				float h = s_top.y - s_bottom.y;
				float h2 = s_top.x - s_bottom.x;
				float w = (s_bottom.y - s_top.y) / 3.5f;
				float w2 = ((s_bottom.y - s_top.y) / 3.5f) * 2;
				auto draw_list = ImGui::GetBackgroundDrawList();

				float text_row = 0;

				if (variables::esp::fillbox) {
					ImColor fillbox_grd_start(
						variables::colors::fillboxcol[0] / 2.f,
						variables::colors::fillboxcol[1] / 2.f,
						variables::colors::fillboxcol[2] / 2.f,
						variables::colors::fillboxcol[3] / 10.f);
					ImColor fillbox_grd_end(
						variables::colors::fillboxcol[0] * 1.5f,
						variables::colors::fillboxcol[1] * 1.5f,
						variables::colors::fillboxcol[2] * 1.5f,
						variables::colors::fillboxcol[3]);

					int vert_start_idx = draw_list->VtxBuffer.Size;
					draw_list->AddRectFilled(ImVec2(s_bottom.x - w, s_top.y), ImVec2(s_bottom.x + w, s_bottom.y), fillbox_grd_end);
					ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, draw_list->VtxBuffer.Size,
						ImVec2(s_bottom.x + (w / 2), s_bottom.y + ((h / 100) * 20)), ImVec2(s_bottom.x + (w / 2), s_bottom.y),
						fillbox_grd_start, fillbox_grd_end);
				}

				if (variables::esp::bones)
				{
					int vert_start_idx = draw_list->VtxBuffer.Size;

					draw_bones(player.bones);

					ImVec2 grd_start, grd_end;
					SDK::WorldToScreen(player.bones.HEAD, &grd_start);
					SDK::WorldToScreen(player.bones.RIGHT_FOOT, &grd_end);

					ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, draw_list->VtxBuffer.Size,
						grd_start, grd_end,
						ImColor(variables::colors::skelcol[0], variables::colors::skelcol[1], variables::colors::skelcol[2], variables::colors::skelcol[3]),
						ImColor(variables::colors::skel_grdcol[0], variables::colors::skel_grdcol[1], variables::colors::skel_grdcol[2], variables::colors::skel_grdcol[3]));
				}

				if (variables::esp::box)
				{
					if (variables::esp::box_outline) {
						ImColor boxoutline(
							variables::colors::box_outlinecol[0],
							variables::colors::box_outlinecol[1],
							variables::colors::box_outlinecol[2],
							variables::colors::box_outlinecol[3]);
						draw_list->AddLine(ImVec2(s_bottom.x - w, s_bottom.y + 1), ImVec2(s_bottom.x - w, s_top.y - 1), boxoutline, 3.f); // лево
						draw_list->AddLine(ImVec2(s_bottom.x + w, s_bottom.y + 1), ImVec2(s_bottom.x + w, s_top.y - 1), boxoutline, 3.f); // право
						draw_list->AddLine(ImVec2(s_bottom.x - w - 1, s_top.y), ImVec2(s_bottom.x + w + 1, s_top.y), boxoutline, 3.f); // верх
						draw_list->AddLine(ImVec2(s_bottom.x - w - 1, s_bottom.y), ImVec2(s_bottom.x + w + 1, s_bottom.y), boxoutline, 3.f); // низ
					}

					ImColor boxcolor(variables::colors::boxcol[0], variables::colors::boxcol[1], variables::colors::boxcol[2], variables::colors::boxcol[3]);
					draw_list->AddLine(ImVec2(s_bottom.x - w, s_bottom.y), ImVec2(s_bottom.x - w, s_top.y), boxcolor, 1.f); // лево
					draw_list->AddLine(ImVec2(s_bottom.x + w, s_bottom.y), ImVec2(s_bottom.x + w, s_top.y), boxcolor, 1.f); // право
					draw_list->AddLine(ImVec2(s_bottom.x - w, s_top.y), ImVec2(s_bottom.x + w, s_top.y), boxcolor, 1.f); // верх
					draw_list->AddLine(ImVec2(s_bottom.x - w, s_bottom.y), ImVec2(s_bottom.x + w, s_bottom.y), boxcolor, 1.f); // низ

					//draw_list->AddRect(ImVec2(s_bottom.x - w, s_top.y), ImVec2(s_bottom.x + w, s_bottom.y), boxcolor);
				}

				if (variables::esp::hp && health > 1.f && health < 101.f)
				{
					float armor = player.armor;
					if (armor >= 100) armor = 100.f;
					if (armor <= 0) armor = 0.f;

					float oneHealth2 = h / 100;
					float oneHealth3 = w2 / 100;
					ImColor hpbarcolor(variables::colors::hpespcol[0], variables::colors::hpespcol[1], variables::colors::hpespcol[2], variables::colors::hpespcol[3]);
					ImColor armbarcolor(variables::colors::hp_arm_espcol[0], variables::colors::hp_arm_espcol[1], variables::colors::hp_arm_espcol[2], variables::colors::hp_arm_espcol[3]);

					switch (variables::esp::hpbar_pos)
					{
					case 0: // left
						draw_hp_bar(draw_list, s_top, s_bottom, health, armor, LEFT);
						if (variables::esp::hptext_bar) {
							if (health > 1 && health < 99)
								outlinedtext(10, std::to_string((int)round(health)), ImVec2(s_bottom.x - w - 5.0f, (s_top.y - h + 2) + oneHealth2 * health), variables::colors::hp_textcol);
						}
						break;

					case 1: // right
						draw_hp_bar(draw_list, s_top, s_bottom, health, armor, RIGHT);
						if (variables::esp::hptext_bar) {
							if (health > 1 && health < 99)
								outlinedtext(10, std::to_string((int)round(health)), ImVec2(s_bottom.x + w + 8.0f, (s_top.y - h + 2) + oneHealth2 * health), variables::colors::hp_textcol);
						}
						break;

					case 2: // btm
						draw_hp_bar(draw_list, s_top, s_bottom, health, armor, BOTTOM);
						if (variables::esp::hptext_bar) {
							if (health > 1 && health < 99)
								outlinedtext(10, std::to_string((int)round(health)), ImVec2((s_bottom.x - w) + oneHealth3 * health, s_bottom.y - 8), variables::colors::hp_textcol);
						}

						if (armor > 1) {
							text_row += 0.5f;
						}

						break;

					case 3: // top
						draw_hp_bar(draw_list, s_top, s_bottom, health, armor, TOP);
						if (variables::esp::hptext_bar) {
							if (health > 1 && health < 99)
								outlinedtext(10, std::to_string((int)round(health)), ImVec2((s_bottom.x - w) + oneHealth3 * health, s_top.y - 11), variables::colors::hp_textcol);
						}
						break;
					}

				}

				if (variables::esp::weapon_esp) {

					if (player.eqpd_weapon_hash != 0) {
						outlinedtext(14, SDK::get_weapon_name(player.eqpd_weapon_hash), ImVec2(s_bottom.x, s_bottom.y + (14 * text_row)), variables::colors::curwepcol);
						text_row++;
					}
				}

				if (variables::esp::distance_esp)
				{
					std::string player_dist_str = std::to_string((int)round(player.distance * 3.28084)) + "FT";
					outlinedtext(14, player_dist_str.c_str(), ImVec2(s_bottom.x, s_bottom.y + (14 * text_row)), variables::colors::distancecol);
					text_row++;
				}
			}
		}
	}
}