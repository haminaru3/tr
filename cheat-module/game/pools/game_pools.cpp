#include "game_pools.hpp"
#include "../../features/misc/sdk.h"
#include "../../features/aimbot/aimbot.h"
#include <mutex>

std::vector<s_player> c_game_pools::players;
std::mutex c_game_pools::players_mutex;

void c_game_pools::update_players() {
    constexpr uint32_t MODEL_MALE_FREEMODE = 0x705E61F2;
    constexpr uint32_t MODEL_FEMALE_FREEMODE = 0x9C9EFFD8;

    uintptr_t replay_interface = SDK::GetReplayInterface();
    if (!_is_valid_ptr(replay_interface)) return;

    uintptr_t list_interface = *(uintptr_t*)(replay_interface + 0x18);
    if (!_is_valid_ptr(list_interface)) return;

    uintptr_t list = *(uintptr_t*)(list_interface + 0x100);
    if (!_is_valid_ptr(list)) return;

    int player_count = *(int*)(list_interface + 0x108);
    if (player_count <= 0 || player_count > 256) return;

    std::vector<s_player> new_players;
    new_players.reserve(player_count);

    for (int i = 0; i < player_count; ++i) {
        uintptr_t ped = *(uintptr_t*)(list + i * 0x10);
        if (!_is_valid_ptr(ped) || ped == 0) continue;

        if (!variables::include_npcs) {
            uintptr_t model_info = *(uintptr_t*)(ped + 0x20);
            if (!_is_valid_ptr(model_info)) continue;

            uint32_t model_hash = *(uint32_t*)(model_info + 0x18);
            if (model_hash != MODEL_MALE_FREEMODE && model_hash != MODEL_FEMALE_FREEMODE) continue;
        }

        s_player player{};
        player.ped = ped;

        Vector3 pos{};
        if (_is_valid_ptr(ped + 0x90)) pos = *(Vector3*)(ped + 0x90);
        player.position = ImVec4(pos.x, pos.y, pos.z, 0.0f);

        player.distance = aimbot::get_distance(pos, SDK::PLAYER::Position());

        if (player.distance <= 0.f || player.distance >= 250.f) continue;

        float health = 0.0f;
        if (_is_valid_ptr(ped + 0x280)) {
            health = *(float*)(ped + 0x280) - 100.0f;
            if (health < 0.0f) health = 0.0f;
        }
        player.health = health;

        float armor = 0.0f;
        if (_is_valid_ptr(ped + 0x150C)) {
            armor = *(float*)(ped + 0x150C);
            if (armor < 0.0f) armor = 0.0f;
        }
        player.armor = armor;

        player.eqpd_weapon_hash = 0;
        if (variables::esp::weapon_esp) {
            uintptr_t weapon_mgr = *(uintptr_t*)(ped + 0x10B8);
            if (_is_valid_ptr(weapon_mgr)) {
                weapon_mgr = *(uintptr_t*)(weapon_mgr + 0x20);
                if (_is_valid_ptr(weapon_mgr)) {
                    uintptr_t weapon_hash = *(uintptr_t*)(weapon_mgr + 0x10);
                    if (weapon_hash && weapon_hash != 0xA2719263)
                        player.eqpd_weapon_hash = weapon_hash;
                }
            }
        }

        //Vector3 raycast_start = SDK::GetBonePosVec3(SDK::LocalPlayer, SDK::RIGHT_HAND);
        //bool is_visible = SDK::RaycastTest(raycast_start, player.bones.HEAD);
        //if (!is_visible) continue;

        if (variables::esp::bones || variables::aimbot::enabled || variables::silent::enabled) {
            ImVec2 tempvec;
            if (SDK::WorldToScreen(Vector3(pos.x, pos.y, pos.z + 0.5f), &tempvec)) {
                BYTE alpha_v = *(BYTE*)((uintptr_t)ped + 0xAC);
                if (alpha_v > 10.f) {
                    collect_bone_data(player);
                }
            }
        }

        new_players.push_back(player);
    }

    {
        std::lock_guard<std::mutex> lock(players_mutex);
        players = std::move(new_players);
    }
}

void c_game_pools::collect_bone_data(s_player& player) {
    if (!_is_valid_ptr(player.ped)) return;

    if (player.distance > 300.f) return;

    if (!variables::esp::bones) {
        if ((GetAsyncKeyState(variables::aimbot::aimkey) & 0x8000) || (GetAsyncKeyState(variables::silent::aimkey) & 0x8000)) {
            player.bones.HEAD = SDK::GetBonePosVec3(player.ped, SDK::HEAD);
            player.bones.NECK = SDK::GetBonePosVec3(player.ped, SDK::NECK);
            player.bones.SPINE3 = SDK::GetBonePosVec3(player.ped, SDK::SPINE3);
        }
        return;
    }

    const std::pair<Vector3&, SDK::e_bones> bone_mappings[] = {
        {player.bones.HEAD, SDK::HEAD},
        {player.bones.NECK, SDK::NECK},
        {player.bones.SPINE1, SDK::SPINE1},
        {player.bones.SPINE2, SDK::SPINE2},
        {player.bones.SPINE3, SDK::SPINE3},
        {player.bones.SPINE_ROOT, SDK::SPINE_ROOT},
        {player.bones.RIGHT_CLAVICLE, SDK::RIGHT_CLAVICLE},
        {player.bones.LEFT_CLAVICLE, SDK::LEFT_CLAVICLE},
        {player.bones.RIGHT_UPPER_ARM, SDK::RIGHT_UPPER_ARM},
        {player.bones.RIGHT_FOREARM, SDK::RIGHT_FOREARM},
        {player.bones.RIGHT_HAND, SDK::RIGHT_HAND},
        {player.bones.LEFT_UPPER_ARM, SDK::LEFT_UPPER_ARM},
        {player.bones.LEFT_FOREARM, SDK::LEFT_FOREARM},
        {player.bones.LEFT_HAND, SDK::LEFT_HAND},
        {player.bones.RIGHT_THIGH, SDK::RIGHT_THIGH},
        {player.bones.RIGHT_CALF, SDK::RIGHT_CALF},
        {player.bones.RIGHT_FOOT, SDK::RIGHT_FOOT},
        {player.bones.LEFT_THIGH, SDK::LEFT_THIGH},
        {player.bones.LEFT_CALF, SDK::LEFT_CALF},
        {player.bones.LEFT_FOOT, SDK::LEFT_FOOT}
    };

    for (const auto& [bone_vec, bone_id] : bone_mappings) {
        bone_vec = SDK::GetBonePosVec3(player.ped, bone_id);
    }
    return;
}

void c_game_pools::retarded_bone_data(s_player& player) {
    player.bones.HEAD = SDK::GetBonePosVec3(player.ped, SDK::HEAD);
    player.bones.NECK = SDK::GetBonePosVec3(player.ped, SDK::NECK);
    player.bones.SPINE1 = SDK::GetBonePosVec3(player.ped, SDK::SPINE1);
    player.bones.SPINE2 = SDK::GetBonePosVec3(player.ped, SDK::SPINE2);
    player.bones.SPINE3 = SDK::GetBonePosVec3(player.ped, SDK::SPINE3);
    player.bones.SPINE_ROOT = SDK::GetBonePosVec3(player.ped, SDK::SPINE_ROOT);

    player.bones.RIGHT_CLAVICLE = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_CLAVICLE);
    player.bones.LEFT_CLAVICLE = SDK::GetBonePosVec3(player.ped, SDK::LEFT_CLAVICLE);

    player.bones.RIGHT_UPPER_ARM = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_UPPER_ARM);
    player.bones.RIGHT_FOREARM = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_FOREARM);
    player.bones.RIGHT_HAND = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_HAND);

    player.bones.LEFT_UPPER_ARM = SDK::GetBonePosVec3(player.ped, SDK::LEFT_UPPER_ARM);
    player.bones.LEFT_FOREARM = SDK::GetBonePosVec3(player.ped, SDK::LEFT_FOREARM);
    player.bones.LEFT_HAND = SDK::GetBonePosVec3(player.ped, SDK::LEFT_HAND);

    player.bones.RIGHT_THIGH = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_THIGH);
    player.bones.RIGHT_CALF = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_CALF);
    player.bones.RIGHT_FOOT = SDK::GetBonePosVec3(player.ped, SDK::RIGHT_FOOT);

    player.bones.LEFT_THIGH = SDK::GetBonePosVec3(player.ped, SDK::LEFT_THIGH);
    player.bones.LEFT_CALF = SDK::GetBonePosVec3(player.ped, SDK::LEFT_CALF);
    player.bones.LEFT_FOOT = SDK::GetBonePosVec3(player.ped, SDK::LEFT_FOOT);
}

void c_game_pools::beginframe_tick() {
    //if (variables::esp::bones || variables::aimbot::enabled || variables::silent::enabled) {
    //    for (auto& player : g_pools.players) {
    //        ImVec2 tempvec;
    //        if (SDK::WorldToScreen(Vector3(player.position.x, player.position.x, player.position.x + 0.5f), &tempvec)) {
    //            collect_bone_data(player);
    //            //retarded_bone_data(player);
    //        }
    //    }
    //}
}