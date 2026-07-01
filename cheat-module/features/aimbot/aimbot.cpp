#pragma once
#include "../misc/sdk.h"
#include "../../game/pools/game_pools.hpp"
#include "../../variables/variables.hpp"
#include "imgui/imgui.h"

namespace aimbot
{
    s_player* _vector_target = nullptr;
    s_player* _silent_target = nullptr;
    static bool _vec_target_exists = false;
    static bool _s_target_exists = false;

    Vector3 silent_aimpos;
    Vector3 aimpos;
    ImVec2 aim2d;

    void clear_vec_target() { _vector_target = nullptr; _vec_target_exists = false; aimpos = Vector3(); aim2d = ImVec2(); }
    void clear_s_target() { _silent_target = nullptr; _s_target_exists = false; silent_aimpos = Vector3(); }

    float screen_distance(float Xx, float Yy, float xX, float yY) {
        return sqrtf((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
    }

    float get_distance(Vector3 to, Vector3 from) {
        return sqrtf(
            ((to.x - from.x) * (to.x - from.x)) +
            ((to.y - from.y) * (to.y - from.y)) +
            ((to.z - from.z) * (to.z - from.z))
        );
    }

    void get_best_target(bool for_silent) {
        bool ignoredead = variables::aimbot::ignoredead;
        float maxdist = variables::aimbot::maxdist;
        float mindist = variables::aimbot::mindist;
        auto bone = variables::aimbot::bone;
        auto fov = variables::aimbot::fov;

        bool* target_status = &_vec_target_exists;
        s_player** target = &_vector_target;

        if (for_silent) {
            bone = variables::silent::bone;
            fov = variables::silent::fov;
            target_status = &_s_target_exists;
            target = &_silent_target;
        }

        if (SDK::PLAYER::Valid()) {
            DWORD64 cam = SDK::GetCameraViewAngles();
            if (Helper::ValidPTR(cam)) {
                static float old_distance = FLT_MAX;
                float new_distance = 0;

                if (*target == nullptr && !(*target_status))
                    old_distance = FLT_MAX;

                for (auto& player : g_pools.players) {
                    if (!_is_valid_ptr(player.ped)) continue;
                    if (player.ped == SDK::LocalPlayer) continue;
                    if (ignoredead && player.health <= 0.f) continue;
                    if (player.distance > maxdist || player.distance < mindist) continue;

                    ImVec2 tempvec;
                    Vector3 target_bone;
                    switch (bone) {
                    case 0: target_bone = player.bones.HEAD; break;
                    case 1: target_bone = player.bones.NECK; break;
                    case 2: target_bone = player.bones.SPINE3; break;
                    default: continue;
                    }

                    if (!SDK::WorldToScreen(target_bone, &tempvec)) continue;

                    ImVec2 display_size = ImGui::GetIO().DisplaySize;
                    new_distance = screen_distance(display_size.x / 2, display_size.y / 2, tempvec.x, tempvec.y);

                    if (new_distance < old_distance && new_distance < fov) {
                        old_distance = new_distance;
                        *target = &player;
                        *target_status = true;
                    }
                }
            }
        }
        else {
            if (for_silent) clear_s_target();
            else clear_vec_target();
        }
    }

    __int64 onBulletInstance(__int64* sBulletInstance_1, const __int64* pWeaponInfo, Vector3* vStart, Vector3* vEnd, float fVelocity, unsigned int weaponHash, bool createsTrace, bool isAccurate) {
        if (GetAsyncKeyState(variables::silent::aimkey) & 0x8000) {
            std::lock_guard<std::mutex> lock(g_pools.players_mutex);

            if (_s_target_exists && (((rand() % 100) + 1) <= variables::silent::hitchance)) {
                if (variables::silent::magicbullet) {
                    vStart->x = silent_aimpos.x + 0.001f;
                    vStart->y = silent_aimpos.y + 0.001f;
                    vStart->z = silent_aimpos.z + 0.1f;
                }

                vEnd->x = silent_aimpos.x;
                vEnd->y = silent_aimpos.y;
                vEnd->z = silent_aimpos.z;
            }
        }

        return SDK::origBulletInstance(sBulletInstance_1, pWeaponInfo, vStart, vEnd, fVelocity, weaponHash, false, true);
    }

    void vector_tick() {
        if (!variables::aimbot::enabled) return;

        std::lock_guard<std::mutex> lock(g_pools.players_mutex);

        if (GetAsyncKeyState(variables::aimbot::aimkey) & 0x8000) {
            DWORD64 Addr = *(DWORD64*)(SDK::LocalPlayer + 0x10B8);
            Addr = *(DWORD64*)(Addr + 0x20);
            uint64_t Hash = *(uint64_t*)(Addr + 0x10);
            if (Hash == NULL || Hash == 0xA2719263) {
                clear_vec_target();
                return;
            }

            DWORD64 cam = SDK::GetCameraViewAngles();
            if (!Helper::ValidPTR(cam)) return;

            if (!_vec_target_exists) get_best_target(false);
            else {
                if (!_is_valid_ptr(_vector_target->ped) || (variables::aimbot::ignoredead && _vector_target->health <= 0.f)) {
                    clear_vec_target();
                    return;
                }

                Vector3 target_bone;
                switch (variables::aimbot::bone) {
                case 0: target_bone = _vector_target->bones.HEAD; break;
                case 1: target_bone = _vector_target->bones.NECK; break;
                case 2: target_bone = _vector_target->bones.SPINE3; break;
                default: clear_vec_target(); return;
                }

                ImVec2 tempvec;
                if (!SDK::WorldToScreen(target_bone, &tempvec)) {
                    clear_vec_target();
                    return;
                }

                ImVec2 display_size = ImGui::GetIO().DisplaySize;
                float dist = screen_distance(display_size.x / 2, display_size.y / 2, tempvec.x, tempvec.y);
                if (dist > variables::aimbot::fov) {
                    clear_vec_target();
                    return;
                }

                aim2d = tempvec;
                aimpos = target_bone;

                Vector3 crosshair_pos = *(Vector3*)(cam + 0x60);
                auto distance = get_distance(crosshair_pos, aimpos);
                Vector3 out_vec = Vector3(
                    (aimpos.x - crosshair_pos.x) / distance,
                    (aimpos.y - crosshair_pos.y) / distance,
                    (aimpos.z - crosshair_pos.z) / distance
                );

                if (variables::aimbot::smothness > 2.0f) {
                    Vector3 view_angles = *(Vector3*)(cam + 0x3D0);
                    Vector3 delta = Vector3(
                        out_vec.x - view_angles.x,
                        out_vec.y - view_angles.y,
                        out_vec.z - view_angles.z
                    );
                    float smooth_factor = 1.f + (variables::aimbot::smothness / 10.f);
                    Vector3 smoothed_angles = Vector3(
                        view_angles.x + (delta.x / smooth_factor),
                        view_angles.y + (delta.y / smooth_factor),
                        view_angles.z + (delta.z / smooth_factor)
                    );

                    *(Vector3*)(cam + 0x3D0) = smoothed_angles;
                    *(Vector3*)(cam + 0x40) = smoothed_angles;
                }
                else {
                    *(Vector3*)(cam + 0x3D0) = out_vec;
                    *(Vector3*)(cam + 0x40) = out_vec;
                }
            }
        }
        else {
            clear_vec_target();
        }
    }

    void silent_tick() {
        if (!variables::silent::enabled) return;

        std::lock_guard<std::mutex> lock(g_pools.players_mutex);

        if (GetAsyncKeyState(variables::silent::aimkey) & 0x8000) {
            DWORD64 Addr = *(DWORD64*)(SDK::LocalPlayer + 0x10B8);
            Addr = *(DWORD64*)(Addr + 0x20);
            uint64_t Hash = *(uint64_t*)(Addr + 0x10);
            if (Hash == NULL || Hash == 0xA2719263) {
                clear_s_target();
                return;
            }

            if (!_s_target_exists) get_best_target(true);
            else {
                if (!_is_valid_ptr(_silent_target->ped) || (variables::aimbot::ignoredead && _silent_target->health <= 0.f)) {
                    clear_s_target();
                    return;
                }

                Vector3 target_bone;
                switch (variables::silent::bone) {
                case 0: target_bone = _silent_target->bones.HEAD; break;
                case 1: target_bone = _silent_target->bones.NECK; break;
                case 2: target_bone = _silent_target->bones.SPINE3; break;
                default: clear_s_target(); return;
                }

                ImVec2 tempvec;
                if (!SDK::WorldToScreen(target_bone, &tempvec)) {
                    clear_s_target();
                    return;
                }

                silent_aimpos = target_bone;
            }
        }
        else {
            clear_s_target();
        }
    }

    void tick() {
        vector_tick();
        silent_tick();
    }
}
