/* settings variables, used by half of all functions */
#pragma once
#include <string>
#include <iostream>

#include "nlohmann/json.hpp"
#include <fstream>
#include <windows.h>
#include "base64.hpp"
#include "imgui/imgui.h"

#define CFG_PATH "C:\\traumaskeet\\"
#define CFG_DIR_PATH "C:\\traumaskeet"

namespace variables
{
	namespace aimbot
	{
		inline bool enabled = false;
		inline int aimkey = 0;
		inline int bone = 0;
		inline bool drawfow = false;
		inline float fov = 0;
		inline float smothness = 0;
		inline int target = 0;
		inline float mindist = 0;
		inline float maxdist = 300;
		inline bool ignoredead = true;
	}

    namespace silent
    {
        inline bool enabled = false;
        inline int aimkey = 0;
        inline int bone = 0;
        inline bool drawfow = false;
        inline float fov = 0;
        inline float hitchance = 50;
        inline bool magicbullet = false;
        inline int target = 0;
    }

	namespace esp
	{
		//int enabled = false;
		inline bool box = false;
		inline bool fillbox = false;
		inline bool corners = false;
		inline bool hp = false;
		inline bool bones = false;
		inline bool snapline = false;
		inline bool distance_esp = false;
		inline bool weapon_esp = false;

		inline int hpbar_pos = 2;

        inline bool box_outline = true;
        inline bool hptext_bar = false;
        inline bool skel_grd = false;
        inline bool hp_grd = false;


		inline float maxdist = 300;
		inline float mindist = 0;
		inline bool ignoredead = true;
		//inline bool ignoreself = true;
	}

	namespace player
	{
		inline bool godmode = false;
		inline bool godmode_state = false;
		inline int gmkey = 0;
		inline bool invisible = false;
		inline bool noclip = false;
		inline int godmodetype = 0;
		inline bool noragdoll = false;
        inline bool nocollision = false;

		inline bool noclip_state = false;
		inline int noclipbind = 0;
		inline int noclipbackward = 0x53;
		inline int noclipforward = 0x57;
        inline int noclipleft = 0x41;
        inline int noclipright = 0x44;
        inline int noclipdown = VK_CONTROL;
        inline int noclipup = VK_SPACE;
		inline int noclipboostbind = 0;
		inline bool noclipboost = false;
		inline float noclipspeed = 10;
	}

	namespace weapon
	{
		inline bool norecoil = false;
		inline bool nospread = false;
		inline bool range = false;
		inline bool noreload = false;
		inline bool bulletintick = false;
        inline bool rapidfire = false;
	}

	namespace menu
	{
		inline bool enabled = false;
		inline bool unhook = false;
	}

	namespace colors
	{
		inline float crosshaircol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float watermarkcol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float drawfovcol[4] = { 1.f, 1.f, 1.f, 1.f };
        inline float drawfovsalocol[4] = { 1.f, 0.f, 0.f, 1.f };

		inline float boxcol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float fillboxcol[4] = { 1.f, 1.f, 1.f, 50.f / 255.f };
		inline float skelcol[4] = { 160 / 255.f, 1.f, 160.f / 255.f, 1.f };
		inline float hpespcol[4] = { 120.f / 255.f, 1.f, 80.f / 255.f, 1.f };
		inline float hp_arm_espcol[4] = { 80.f / 255.f, 140.f / 255.f, 200.f / 255.f, 1.f };
		inline float snaplinescol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float distancecol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float espflagcol[4] = { 1.f, 1.f, 1.f, 1.f };
		inline float curwepcol[4] = { 1.f, 1.f, 1.f, 1.f };

        inline float box_outlinecol[4] = { 0.f, 0.f, 0.f, 1.f };
        inline float hp_textcol[4] = { 1.f, 1.f, 1.f, 1.f };
        inline float skel_grdcol[4] = { 160 / 255.f, 1.f, 160.f / 255.f, 1.f };
        inline float hp_grdcol[4] = { 120.f / 255.f, 1.f, 80.f / 255.f, 1.f };
        inline float radar_markcol[4] = { 159.f / 255.f, 202.f / 255.f, 43.f / 255.f, 1.f };
	}

	namespace visual_misc
	{
		inline bool crosshair = false;
		inline int crosshair_type = 0;
		inline float crosshair_size = 4;
        inline float nazi_cross_size = 8;

        inline bool watermark = false;
        inline bool wm_elements[3] = { true, false, false };

        inline bool radar = false;
		inline bool cordswatermark = false;
	}

    inline bool include_npcs = false;
}

namespace cfg_mgmt {
    using json = nlohmann::json;

    inline std::string encrypt(const std::string& input)
    {
        std::string encoded = base64::to_base64(input);
        for (size_t i = 0; i < encoded.length(); i++)
        {
            encoded[i] ^= (i % 5) + 3;
        }
        return base64::to_base64(encoded);
    }

    inline std::string decrypt(const std::string& input)
    {
        std::string decoded = base64::from_base64(input);
        for (size_t i = 0; i < decoded.length(); i++)
        {
            decoded[i] ^= (i % 5) + 3;
        }
        return base64::from_base64(decoded);
    }

    inline void saveSettings(const std::string& filename)
    {
        std::string path = CFG_PATH + filename;
        json j;

        j["aimbot"] = {
            {"enabled", variables::aimbot::enabled},
            {"aimkey", variables::aimbot::aimkey},
            {"bone", variables::aimbot::bone},
            {"drawfow", variables::aimbot::drawfow},
            {"fov", variables::aimbot::fov},
            {"smothness", variables::aimbot::smothness},
            //{"target", variables::aimbot::target},
            {"mindist", variables::aimbot::mindist},
            {"maxdist", variables::aimbot::maxdist},
            {"ignoredead", variables::aimbot::ignoredead},
            //{"npcs", variables::aimbot::npcs}
        };

        j["silent"] = {
            {"enabled", variables::silent::enabled},
            {"aimkey", variables::silent::aimkey},
            {"bone", variables::silent::bone},
            {"drawfow", variables::silent::drawfow},
            {"fov", variables::silent::fov},
            {"hitchance", variables::silent::hitchance},
            {"magicbullet", variables::silent::magicbullet},
            //{"target", variables::silent::target}
        };

        j["esp"] = {
            {"box", variables::esp::box},
            {"fillbox", variables::esp::fillbox},
            {"corners", variables::esp::corners},
            {"hp", variables::esp::hp},
            {"bones", variables::esp::bones},
            {"snapline", variables::esp::snapline},
            {"distance_esp", variables::esp::distance_esp},
            {"weapon_esp", variables::esp::weapon_esp},
            {"hptext_bar", variables::esp::hptext_bar},
            {"hpbar_pos", variables::esp::hpbar_pos},
            {"maxdist", variables::esp::maxdist},
            {"mindist", variables::esp::mindist},
            {"ignoredead", variables::esp::ignoredead},
            //{"ignoreself", variables::esp::ignoreself},

            {"box_outline", variables::esp::box_outline},
            {"skel_grd", variables::esp::skel_grd},
            {"hp_grd", variables::esp::hp_grd},
            //{"npcs", variables::esp::npcs}
        };

        j["player"] = {
            {"godmode", variables::player::godmode},
            //{"godmode_state", variables::player::godmode_state},
            {"gmkey", variables::player::gmkey},
            {"invisible", variables::player::invisible},
            {"noclip", variables::player::noclip},
            {"godmodetype", variables::player::godmodetype},
            {"nocollision", variables::player::nocollision},
            {"noragdoll", variables::player::noragdoll},
            //{"NoclipState", variables::player::NoclipState},
            {"noclipbind", variables::player::noclipbind},
            {"noclipbackward", variables::player::noclipbackward},
            {"noclipforward", variables::player::noclipforward},
            {"noclipleft", variables::player::noclipleft},
            {"noclipright", variables::player::noclipright},
            {"noclipdown", variables::player::noclipdown},
            {"noclipup", variables::player::noclipup},
            {"noclipboostbind", variables::player::noclipboostbind},
            {"noclipboost", variables::player::noclipboost},
            {"noclipspeed", variables::player::noclipspeed}
        };

        j["weapon"] = {
            {"norecoil", variables::weapon::norecoil},
            {"nospread", variables::weapon::nospread},
            {"range", variables::weapon::range},
            {"noreload", variables::weapon::noreload},
            {"bulletintick", variables::weapon::bulletintick},
            {"rapidfire", variables::weapon::rapidfire}
        };

        j["colors"] = {
            {"crosshaircol", variables::colors::crosshaircol},
            {"watermarkcol", variables::colors::watermarkcol},
            {"drawfovcol", variables::colors::drawfovcol},
            {"drawfovsalocol", variables::colors::drawfovsalocol},
            {"boxcol", variables::colors::boxcol},
            {"fillboxcol", variables::colors::fillboxcol},
            {"skelcol", variables::colors::skelcol},
            {"hpespcol", variables::colors::hpespcol},
            {"hp_arm_espcol", variables::colors::hp_arm_espcol},
            {"snaplinescol", variables::colors::snaplinescol},
            {"distancecol", variables::colors::distancecol},
            {"espflagcol", variables::colors::espflagcol},
            {"curwepcol", variables::colors::curwepcol},

            {"box_outlinecol", variables::colors::box_outlinecol},
            {"hp_textcol", variables::colors::hp_textcol},
            {"skel_grdcol", variables::colors::skel_grdcol},
            {"hp_grdcol", variables::colors::hp_grdcol},

            {"radar_markcol", variables::colors::radar_markcol},
        };

        j["visual_misc"] = {
            {"crosshair", variables::visual_misc::crosshair},
            {"crosshair_type", variables::visual_misc::crosshair_type},
            {"crosshair_size", variables::visual_misc::crosshair_size},
            {"nazi_cross_size", variables::visual_misc::nazi_cross_size},

            {"watermark", variables::visual_misc::watermark},
            {"wm_show_build", variables::visual_misc::wm_elements[0]},
            {"wm_show_fps", variables::visual_misc::wm_elements[1]},
            {"wm_show_time", variables::visual_misc::wm_elements[2]},

            {"radar", variables::visual_misc::radar},
            {"cordswatermark", variables::visual_misc::cordswatermark}
        };

        std::string imgui_ini_data = ImGui::SaveIniSettingsToMemory();
        j["sosal"] = {
            {"imgui_ini_data", imgui_ini_data}
        };

        std::string jsonString = j.dump();
        std::string encryptedData = encrypt(jsonString);

        std::ofstream file(path, std::ios::out);
        if (file.is_open())
        {
            file << encryptedData;
            file.close();
        }
    }

    inline void loadSettings(const std::string& filename) {
        std::string path = CFG_PATH + filename;
        std::ifstream file(path, std::ios::in);

        if (!file.is_open()) {
            MessageBoxA(0, "failed to open cfg file", "err", MB_ICONERROR);
            return;
        }

        std::string encryptedData(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
        file.close();

        //try {
        {
            std::string decryptedJson = decrypt(encryptedData);
            json j = json::parse(decryptedJson);

            // Aimbot
            variables::aimbot::enabled = j["aimbot"]["enabled"].get<bool>();
            variables::aimbot::aimkey = j["aimbot"]["aimkey"].get<int>();
            variables::aimbot::bone = j["aimbot"]["bone"].get<int>();
            variables::aimbot::drawfow = j["aimbot"]["drawfow"].get<bool>();
            variables::aimbot::fov = j["aimbot"]["fov"].get<float>();
            variables::aimbot::smothness = j["aimbot"]["smothness"].get<float>();
            //variables::aimbot::target = j["aimbot"]["target"].get<int>();
            variables::aimbot::mindist = j["aimbot"]["mindist"].get<float>();
            variables::aimbot::maxdist = j["aimbot"]["maxdist"].get<float>();
            variables::aimbot::ignoredead = j["aimbot"]["ignoredead"].get<bool>();
            //variables::aimbot::npcs = j["aimbot"]["npcs"].get<bool>();

            // Silent
            variables::silent::enabled = j["silent"]["enabled"].get<bool>();
            variables::silent::aimkey = j["silent"]["aimkey"].get<int>();
            variables::silent::bone = j["silent"]["bone"].get<int>();
            variables::silent::drawfow = j["silent"]["drawfow"].get<bool>();
            variables::silent::fov = j["silent"]["fov"].get<float>();
            variables::silent::hitchance = j["silent"]["hitchance"].get<float>();
            variables::silent::magicbullet = j["silent"]["magicbullet"].get<bool>();
            //variables::silent::target = j["silent"]["target"].get<int>();

            // ESP
            variables::esp::box = j["esp"]["box"].get<bool>();
            variables::esp::fillbox = j["esp"]["fillbox"].get<bool>();
            variables::esp::corners = j["esp"]["corners"].get<bool>();
            variables::esp::hp = j["esp"]["hp"].get<bool>();
            variables::esp::bones = j["esp"]["bones"].get<bool>();
            variables::esp::snapline = j["esp"]["snapline"].get<bool>();
            variables::esp::distance_esp = j["esp"]["distance_esp"].get<bool>();
            variables::esp::weapon_esp = j["esp"]["weapon_esp"].get<bool>();
            variables::esp::hptext_bar = j["esp"]["hptext_bar"].get<bool>();
            variables::esp::hpbar_pos = j["esp"]["hpbar_pos"].get<int>();
            variables::esp::maxdist = j["esp"]["maxdist"].get<float>();
            variables::esp::mindist = j["esp"]["mindist"].get<float>();
            variables::esp::ignoredead = j["esp"]["ignoredead"].get<bool>();

            variables::esp::box_outline = j["esp"]["box_outline"].get<bool>();
            variables::esp::skel_grd = j["esp"]["skel_grd"].get<bool>();
            variables::esp::hp_grd = j["esp"]["hp_grd"].get<bool>();
            //variables::esp::ignoreself = j["esp"]["ignoreself"].get<bool>();
            //variables::esp::npcs = j["esp"]["npcs"].get<bool>();

            // Player
            variables::player::godmode = j["player"]["godmode"].get<bool>();
            //variables::player::godmode_state = j["player"]["godmode_state"].get<bool>();
            variables::player::gmkey = j["player"]["gmkey"].get<int>();
            variables::player::invisible = j["player"]["invisible"].get<bool>();
            variables::player::noclip = j["player"]["noclip"].get<bool>();
            variables::player::godmodetype = j["player"]["godmodetype"].get<int>();
            variables::player::nocollision = j["player"]["nocollision"].get<bool>();
            variables::player::noragdoll = j["player"]["noragdoll"].get<bool>();
            //variables::player::NoclipState = j["player"]["NoclipState"].get<bool>();
            variables::player::noclipbind = j["player"]["noclipbind"].get<int>();
            variables::player::noclipbackward = j["player"]["noclipbackward"].get<int>();
            variables::player::noclipforward = j["player"]["noclipforward"].get<int>();
            variables::player::noclipleft = j["player"]["noclipleft"].get<int>();
            variables::player::noclipright = j["player"]["noclipright"].get<int>();
            variables::player::noclipdown = j["player"]["noclipdown"].get<int>();
            variables::player::noclipup = j["player"]["noclipup"].get<int>();
            variables::player::noclipboostbind = j["player"]["noclipboostbind"].get<int>();
            variables::player::noclipboost = j["player"]["noclipboost"].get<bool>();
            variables::player::noclipspeed = j["player"]["noclipspeed"].get<float>();

            // Weapon
            variables::weapon::norecoil = j["weapon"]["norecoil"].get<bool>();
            variables::weapon::nospread = j["weapon"]["nospread"].get<bool>();
            variables::weapon::range = j["weapon"]["range"].get<bool>();
            variables::weapon::noreload = j["weapon"]["noreload"].get<bool>();
            variables::weapon::bulletintick = j["weapon"]["bulletintick"].get<bool>();
            variables::weapon::rapidfire = j["weapon"]["rapidfire"].get<bool>();

            // Colors
            auto& colors = j["colors"];
            for (int i = 0; i < 4; i++) {
                variables::colors::crosshaircol[i] = colors["crosshaircol"][i].get<float>();
                variables::colors::watermarkcol[i] = colors["watermarkcol"][i].get<float>();
                variables::colors::drawfovcol[i] = colors["drawfovcol"][i].get<float>();
                variables::colors::drawfovsalocol[i] = colors["drawfovsalocol"][i].get<float>();
                variables::colors::boxcol[i] = colors["boxcol"][i].get<float>();
                variables::colors::fillboxcol[i] = colors["fillboxcol"][i].get<float>();
                variables::colors::skelcol[i] = colors["skelcol"][i].get<float>();
                variables::colors::hpespcol[i] = colors["hpespcol"][i].get<float>();
                variables::colors::hp_arm_espcol[i] = colors["hp_arm_espcol"][i].get<float>();
                variables::colors::snaplinescol[i] = colors["snaplinescol"][i].get<float>();
                variables::colors::distancecol[i] = colors["distancecol"][i].get<float>();
                variables::colors::espflagcol[i] = colors["espflagcol"][i].get<float>();
                variables::colors::curwepcol[i] = colors["curwepcol"][i].get<float>();

                variables::colors::box_outlinecol[i] = colors["box_outlinecol"][i].get<float>();
                variables::colors::hp_textcol[i] = colors["hp_textcol"][i].get<float>();
                variables::colors::skel_grdcol[i] = colors["skel_grdcol"][i].get<float>();
                variables::colors::hp_grdcol[i] = colors["hp_grdcol"][i].get<float>();
                variables::colors::radar_markcol[i] = colors["radar_markcol"][i].get<float>();
            }

            // Visual Misc
            variables::visual_misc::crosshair = j["visual_misc"]["crosshair"].get<bool>();
            variables::visual_misc::crosshair_type = j["visual_misc"]["crosshair_type"].get<int>();
            variables::visual_misc::crosshair_size = j["visual_misc"]["crosshair_size"].get<float>();
            variables::visual_misc::nazi_cross_size = j["visual_misc"]["nazi_cross_size"].get<float>();

            variables::visual_misc::watermark = j["visual_misc"]["watermark"].get<bool>();
            variables::visual_misc::wm_elements[0] = j["visual_misc"]["wm_show_build"].get<bool>();
            variables::visual_misc::wm_elements[1] = j["visual_misc"]["wm_show_fps"].get<bool>();
            variables::visual_misc::wm_elements[2] = j["visual_misc"]["wm_show_time"].get<bool>();

            variables::visual_misc::radar = j["visual_misc"]["radar"].get<bool>();
            variables::visual_misc::cordswatermark = j["visual_misc"]["cordswatermark"].get<bool>();

            ImGui::LoadIniSettingsFromMemory(j["sosal"]["imgui_ini_data"].get<std::string>().c_str());
        }
        //catch (const json::exception& e) {
        //    MessageBoxA(0, ("JSON Error: " + std::string(e.what())).c_str(), "Error", MB_ICONERROR);
        //}
        //catch (...) {
        //    MessageBoxA(0, "Unknown error while loading settings!", "Error", MB_ICONERROR);
        //}
    }
}