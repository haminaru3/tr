
#include "visual_misc.h"
#include "../game/pools/game_pools.hpp"
#include "../features/misc/sdk.h"
#include "../prot_wrapper.h"

#include <windows.h>

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

ImVec2 world_to_radar(Vector3 position, float radar_x, float radar_y, ImVec2 radar_wnd_size, float zoom) {
    ImVec2 return_pos(0, 0);
    uintptr_t cam = SDK::GetCameraViewAngles();
    if (Helper::ValidPTR(cam) && Helper::ValidPTR(SDK::LocalPlayer)) {
        Vector3 fps_angles = *(Vector3*)(cam + 0x40);
        Vector3 lp_pos = SDK::PLAYER::Position();

        float rot = atan2(fps_angles.y, fps_angles.x);

        Vector2 forward_vec({ cos(rot), sin(rot) });

        float delta_x = position.x - lp_pos.x;
        float delta_y = position.y - lp_pos.y;

        float position_x = (delta_y * forward_vec.x - delta_x * forward_vec.y) / zoom;
        float position_y = (delta_x * forward_vec.x + delta_y * forward_vec.y) / zoom;

        if (fabs(position_x) > radar_wnd_size.x / 2.0f || fabs(position_y) > radar_wnd_size.y / 2.0f) {
            return ImVec2(0, 0);
        }

        position_x = std::clamp(position_x, -(radar_wnd_size.x / 2.0f - 2.5f), radar_wnd_size.x / 2.0f - 2.5f);
        position_y = std::clamp(position_y, -(radar_wnd_size.y / 2.0f - 2.5f), radar_wnd_size.y / 2.0f - 2.5f);

        return_pos.x = radar_x - position_x;
        return_pos.y = radar_y - position_y;
    }

    return return_pos;
}

namespace visual_misc {

	void default_crosshair(int size) {
		auto MiddleX = ImGui::GetIO().DisplaySize.x / 2; auto MiddleY = ImGui::GetIO().DisplaySize.y / 2;
        ImColor col = ImColor(variables::colors::crosshaircol[0], variables::colors::crosshaircol[1], variables::colors::crosshaircol[2], variables::colors::crosshaircol[3]);
		ImGui::GetBackgroundDrawList()->AddLine({ MiddleX - size, MiddleY }, { MiddleX + size + 1, MiddleY }, col);
		ImGui::GetBackgroundDrawList()->AddLine({ MiddleX, MiddleY - size }, { MiddleX, MiddleY + size + 1 }, col);
	}

    int rotate = 0;
	void nazi_crosshair(int size) { // pasted from crc32
        ImColor col = ImColor(variables::colors::crosshaircol[0], variables::colors::crosshaircol[1], variables::colors::crosshaircol[2], variables::colors::crosshaircol[3]);
		auto MiddleX = ImGui::GetIO().DisplaySize.x / 2; auto MiddleY = ImGui::GetIO().DisplaySize.y / 2;
		int a = size;
		float gamma = atan(a / a);

        if (rotate >= 360) rotate = 0;
        else rotate++;

		int i = 0;
		while (i < 4)
		{
			std::vector <int> p;
			p.push_back(a * sin(GRD_TO_BOG(rotate + (i * 90))));
			p.push_back(a * cos(GRD_TO_BOG(rotate + (i * 90))));
			p.push_back((a / cos(gamma)) * sin(GRD_TO_BOG(rotate + (i * 90) + BOG_TO_GRD(gamma))));
			p.push_back((a / cos(gamma)) * cos(GRD_TO_BOG(rotate + (i * 90) + BOG_TO_GRD(gamma))));

			ImGui::GetBackgroundDrawList()->AddLine({ MiddleX, MiddleY }, { MiddleX + p[0], MiddleY - p[1] }, col);
			ImGui::GetBackgroundDrawList()->AddLine({ MiddleX + p[0], MiddleY - p[1] }, { MiddleX + p[2], MiddleY - p[3] }, col);

			i++;
		}
	}

    void x_cross(int size)
    {
        auto MiddleX = ImGui::GetIO().DisplaySize.x / 2; auto MiddleY = ImGui::GetIO().DisplaySize.y / 2;
        ImColor col = ImColor(variables::colors::crosshaircol[0], variables::colors::crosshaircol[1], variables::colors::crosshaircol[2], variables::colors::crosshaircol[3]);

        ImGui::GetBackgroundDrawList()->AddLine({ MiddleX - size, MiddleY - size }, { MiddleX + size, MiddleY + size + 1 }, col);
        ImGui::GetBackgroundDrawList()->AddLine({ MiddleX + size, MiddleY - size - 1 }, { MiddleX - size, MiddleY + size }, col);
    }

	void fov_circle(float col[4], float rad) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        ImVec2 draw_pos({ display_size.x / 2, display_size.y / 2 });
        ImColor draw_col(col[0], col[1], col[2], col[3]);

        //float rad_anim = ImLerp(rad_anim, rad, ImGui::GetIO().DeltaTime * 14.f);
        if (rad > 1)
            draw->AddCircle(draw_pos, rad, draw_col, 360, 1.f);
	}

    class s_watermark_text {
    private:
        std::string game = "?";
        std::string fps;
        std::string time;

        float get_fps() {
            static int frameCount = 0;
            static float timeElapsed = 0.0f;
            static float _fps = 0.0f;

            frameCount++;
            timeElapsed += ImGui::GetIO().DeltaTime;

            if (timeElapsed >= 1.0f) {
                _fps = (float)frameCount / timeElapsed;
                frameCount = 0;
                timeElapsed = 0.0f;
            }
            return _fps / 2;
        }

        std::string get_time() {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream tss;
            tss << std::put_time(&tm, "%H:%M:%S");
            return tss.str();
        }

        std::string get_game() {
            if (LI_CALL(GetModuleHandleA)("multiplayer.dll")) return "ragemp";
            else if (LI_CALL(GetModuleHandleA)("altv-client.dll")) return "altv";
            else return "gtav";
        }

    public:
        const std::string label_first = "trauma", label_second = "skeet";
        std::string all_text;
        std::string draw_text;

        s_watermark_text() {
            fps = std::to_string((int)get_fps()) + "fps";
            time = get_time();
            game = get_game();

            all_text = label_first + label_second;
            draw_text = label_second;
            if (variables::visual_misc::wm_elements[0]) all_text += " | " + game, draw_text += " | " + game;
            if (variables::visual_misc::wm_elements[1]) all_text += " | " + fps, draw_text += " | " + fps;
            if (variables::visual_misc::wm_elements[2]) all_text += " | " + time, draw_text += " | " + time;
        }
    };

    static enum e_colors {
        BACKGROUND,
        OUTLINE,
        INNER_OUTLINE,
        INNER_BOX,
        ACCENT,
        TEXT,
        COUNT
    };

    const ImColor colors[COUNT] = {
        ImColor(28, 28, 28),    // BACKGROUND
        ImColor(0, 0, 0),       // OUTLINE
        ImColor(53, 53, 53),    // INNER_OUTLINE
        ImColor(0, 0, 0),       // INNER_BOX
        ImColor(159, 202, 43),  // ACCENT
        ImColor(255, 255, 255)  // TEXT
    };

    const ImColor grd_col_1[] = {
        ImColor(59, 175, 222, 255),
        ImColor(202, 70, 205, 255),
        ImColor(201, 227, 58, 255)
    };
    const ImColor grd_col_2[] = {
        ImColor(59, 175, 222, 130),
        ImColor(202, 70, 205, 130),
        ImColor(201, 227, 58, 130)
    };

    void draw_grd_line(ImVec2& p1, ImVec2& p2, ImDrawList* draw, ImVec2 pos) {
        //ImVec2 p1(margins + ImVec2(1, 1));
        //ImVec2 p2((size - margins) - ImVec2(0, 17) - ImVec2(1, 1));

        auto start_grd = [&](int& idx, ImDrawList* dl) { idx = dl->VtxBuffer.Size; };
        auto end_grd = [&](int& idx, ImDrawList* dl, ImVec2 start_pos, ImVec2 end_pos, ImColor start_col, ImColor end_col) {
            ImGui::ShadeVertsLinearColorGradientKeepAlpha(dl, idx, dl->VtxBuffer.Size, start_pos, end_pos, start_col, end_col);
            };

        int vert_start_idx;
        {
            start_grd(vert_start_idx, draw);
            draw->AddLine(pos + ImVec2(p1.x, p2.y), pos + ImVec2(p2.x / 2, p2.y), grd_col_1[0]);
            end_grd(vert_start_idx, draw, pos + ImVec2(p1.x, p1.y), pos + ImVec2(p2.x / 2, p2.y), grd_col_1[0], grd_col_1[1]);

            start_grd(vert_start_idx, draw);
            draw->AddLine(pos + ImVec2(p2.x / 2, p2.y), pos + ImVec2(p2.x, p2.y), grd_col_1[1]);
            end_grd(vert_start_idx, draw, pos + ImVec2(p2.x / 2, p1.y), pos + ImVec2(p2.x, p2.y), grd_col_1[1], grd_col_1[2]);
        }
        {
            p2.y += 1;
            start_grd(vert_start_idx, draw);
            draw->AddLine(pos + ImVec2(p1.x, p2.y), pos + ImVec2(p2.x / 2, p2.y), grd_col_2[0]);
            end_grd(vert_start_idx, draw, pos + ImVec2(p1.x, p1.y), pos + ImVec2(p2.x / 2, p2.y), grd_col_2[0], grd_col_2[1]);

            start_grd(vert_start_idx, draw);
            draw->AddLine(pos + ImVec2(p2.x / 2, p2.y), pos + ImVec2(p2.x, p2.y), grd_col_2[1]);
            end_grd(vert_start_idx, draw, pos + ImVec2(p2.x / 2, p1.y), pos + ImVec2(p2.x, p2.y), grd_col_2[1], grd_col_2[2]);
        }

    }

    void draw_watermark() {
        s_watermark_text g_watermark_text;

        auto calc_text_size = [&](std::string text) {
            return ImGui::GetFont()->CalcTextSizeA(11.f, FLT_MAX, 0, text.c_str());
            };

        ImVec2 margins({ 6,6 });
        ImVec2 begin_size = ImVec2((margins.x * 2) + 12 + calc_text_size(g_watermark_text.all_text).x, 32);

        ImGui::SetNextWindowSize(begin_size);

        static bool first_use = true;
        if (first_use) {
            ImVec2 start_pos = ImVec2(ImGui::GetIO().DisplaySize.x - 30, 30);
            start_pos.x -= begin_size.x;
            ImGui::SetNextWindowPos(start_pos, ImGuiCond_FirstUseEver);
        }

        ImGui::Begin(xor("##watermark"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground); {

            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 size = ImGui::GetWindowSize();
            ImVec2 pos = ImGui::GetWindowPos();

            auto add_rect = [&](ImVec2 p1, ImVec2 p2, e_colors col, bool filled = false) {
                filled ? draw->AddRectFilled(pos + p1, pos + p2, colors[col])
                    : draw->AddRect(pos + p1, pos + p2, colors[col], 0, 0, 1.f);
                };

            add_rect({ 1, 1 }, size + ImVec2(1, 1), BACKGROUND, true);
            add_rect({ 0, 0 }, size, OUTLINE);

            add_rect(margins, (size - margins), INNER_BOX, true);
            add_rect({ 1, 1 }, (size - ImVec2(1, 1)), INNER_OUTLINE);
            add_rect(margins, (size - margins), INNER_OUTLINE);

            {
                ImVec2 p1(margins + ImVec2(1, 1));
                ImVec2 p2((size - margins) - ImVec2(0, 17) - ImVec2(1, 1));
                draw_grd_line(p1, p2, draw, pos);
            }

            auto add_text = [&](std::string text, ImVec2 p, e_colors col) { draw->AddText(ImGui::GetFont(), 11.f, pos + p, colors[col], text.c_str()); };

            ImVec2 text_start(margins + ImVec2(5, 5));

            add_text(g_watermark_text.label_first, text_start, ACCENT);
            add_text(g_watermark_text.draw_text, ImVec2(calc_text_size(g_watermark_text.label_first).x + 1, 0) + text_start, TEXT);


        } ImGui::End();
    }

    void draw_radar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 150.f,150.f });
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(400, 400));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, (0, 0, 0, 0));
        //ImGui::PushStyleColor(ImGuiCol_Border, (0, 0, 0, 0));

        ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

        auto rad_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
        if (!variables::menu::enabled) { rad_flags |= ImGuiWindowFlags_NoResize; };
        ImGui::Begin(xor("##radar"), NULL, rad_flags); {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 size = ImGui::GetWindowSize();
            ImVec2 pos = ImGui::GetWindowPos();

            auto add_rect = [&](ImVec2 p1, ImVec2 p2, e_colors col, bool filled = false, float thickness = 1.f) {
                filled ? draw->AddRectFilled(pos + p1, pos + p2, colors[col])
                    : draw->AddRect(pos + p1, pos + p2, colors[col], 0, 0, thickness);
                };

            ImVec2 regn_min = ImGui::GetWindowContentRegionMin(); ImVec2 regn_max = ImGui::GetWindowContentRegionMax();
            ImVec2 vMin = ImVec2(regn_min.x + 8, regn_min.y + 12);
            ImVec2 vMax = ImVec2(regn_max.x - 8, regn_max.y - 8);
            vMin.x += pos.x;vMin.y += pos.y;
            vMax.x += pos.x;vMax.y += pos.y;
            ImVec2 Center = ImVec2(vMin.x + (size.x / 2), vMin.y + (size.y / 2));

            draw->AddRectFilled(pos, pos + size, ImColor(0, 0, 0, 180));
            draw->AddLine(ImVec2(pos.x + size.x / 2, pos.y + 2), ImVec2(pos.x + size.x / 2, pos.y + size.y - 2), ImColor(255, 255, 255, 30));
            draw->AddLine(ImVec2(pos.x + 2, pos.y + size.y / 2), ImVec2(pos.x + size.x - 2, pos.y + size.y / 2 - 1), ImColor(255, 255, 255, 30));
            
            ImVec2 margins({ 6,6 });

            {
                ImVec2 p1(margins + ImVec2(1, 1));
                ImVec2 p2(size.x - margins.x - 1, margins.y + 2);
                draw->AddRectFilled(pos + p1 - ImVec2(1, 1), pos + p2 + ImVec2(1, 4), ImColor(0, 0, 0));
                draw_grd_line(p1, p2, draw, pos);
            }

            std::lock_guard<std::mutex> lock(g_pools.players_mutex);

            for (auto& player : g_pools.players) {
                if (!_is_valid_ptr(player.ped)) continue;
                if (player.ped == 0) continue;

                if (variables::esp::ignoredead && player.health <= 0.f) continue;

                if (!SDK::LocalPlayer)
                    return;
                if (player.ped == SDK::LocalPlayer) continue;

                Vector3 pedPos = Vector3(player.position.x, player.position.y, player.position.z);
                ImVec2 RadarPos = world_to_radar(pedPos, Center.x, Center.y, size, 1.f);

                ImColor mark_col(variables::colors::radar_markcol[0], variables::colors::radar_markcol[1], variables::colors::radar_markcol[2], variables::colors::radar_markcol[3]);
                draw->AddCircleFilled(RadarPos, 3.f, ImColor(0, 0, 0, 255), 16);
                draw->AddCircleFilled(RadarPos, 1.5f, mark_col, 16.f);

            }

            add_rect({ 2, 2 }, size - ImVec2(2, 2), BACKGROUND, false, 10.f);
            add_rect({ 0, 0 }, size, OUTLINE);

            add_rect({ 1, 1 }, (size - ImVec2(1, 1)), INNER_OUTLINE);
            add_rect(margins, (size - margins), INNER_OUTLINE);
        };
        ImGui::End();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
    }

    void init_all() {
        if (variables::visual_misc::radar) draw_radar();
        if (variables::visual_misc::watermark) draw_watermark();
        if (variables::visual_misc::crosshair) {
            switch (variables::visual_misc::crosshair_type)
            {
            case 0:
                default_crosshair(variables::visual_misc::crosshair_size); break;
            case 1:
                x_cross(variables::visual_misc::crosshair_size); break;
            case 2:
                nazi_crosshair(variables::visual_misc::nazi_cross_size); break;
            }
        }

        if (variables::aimbot::enabled && variables::aimbot::drawfow)
            fov_circle(variables::colors::drawfovcol, variables::aimbot::fov);
        if (variables::silent::enabled && variables::silent::drawfow)
            fov_circle(variables::colors::drawfovsalocol, variables::silent::fov);
    }
}