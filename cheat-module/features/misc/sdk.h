#pragma once
#include <Windows.h>
#include "imgui/imgui.h"
#include "../../memory/math.h"
#include "../../memory/helper.h"
#include <xmmintrin.h>
#include "../noclip/noclip.hpp"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

struct s_entity_bones {
	Vector3 HEAD;
	Vector3 NECK;

	Vector3 RIGHT_HAND;
	Vector3 RIGHT_FOREARM;
	Vector3 RIGHT_UPPER_ARM;
	Vector3 RIGHT_CLAVICLE;

	Vector3 LEFT_HAND;
	Vector3 LEFT_FOREARM;
	Vector3 LEFT_UPPER_ARM;
	Vector3 LEFT_CLAVICLE;

	Vector3 PELVIS;
	Vector3 SPINE_ROOT;
	Vector3 SPINE0;
	Vector3 SPINE1;
	Vector3 SPINE2;
	Vector3 SPINE3;

	Vector3 RIGHT_TOE;
	Vector3	RIGHT_FOOT;
	Vector3	RIGHT_CALF;
	Vector3	RIGHT_THIGH;

	Vector3	LEFT_TOE;
	Vector3	LEFT_FOOT;
	Vector3	LEFT_CALF;
	Vector3	LEFT_THIGH;
};

namespace SDK
{
	extern bool WorldToScreen(Vector3 pos, ImVec2* out);

	extern uintptr_t GetCameraViewAngles();
	extern uintptr_t GetReplayInterface();

	enum e_bones : int32_t {
		HEAD = 0x796E,
		NECK = 0x9995,
		SPINE1 = 0x60F0,
		SPINE2 = 0x60F1,
		SPINE3 = 0x60F2,
		SPINE_ROOT = 0xE0FD,
		RIGHT_CLAVICLE = 0x29D2,
		LEFT_CLAVICLE = 0xFCD9,
		RIGHT_UPPER_ARM = 0x9D4D,
		RIGHT_FOREARM = 0x6E5C,
		RIGHT_HAND = 0xDEAD,
		LEFT_UPPER_ARM = 0xB1C5,
		LEFT_FOREARM = 0xEEEB,
		LEFT_HAND = 0x49D9,
		RIGHT_THIGH = 0xCA72,
		RIGHT_CALF = 0x9000,
		RIGHT_FOOT = 0xCC4D,
		LEFT_THIGH = 0xE39F,
		LEFT_CALF = 0xF9BB,
		LEFT_FOOT = 0x3779
	};

	ImVec2 GetBonePosW2S(uintptr_t ped, int32_t wMask);
	Vector3 GetBonePosVec3(uintptr_t ped, const int32_t bone_mask);
	Vector3 GetBonePosVec3(uintptr_t ped, e_bones bone);

	bool RaycastTest(Vector3 start_p, Vector3 end_p);

	extern uintptr_t World;
	extern uintptr_t LocalPlayer;

	typedef __int64(__fastcall* bulletinstance_t)(
		__int64* sBulletInstance_1,
		const __int64* pWeaponInfo,
		Vector3* vStart,
		Vector3* vEnd,
		float fVelocity,
		unsigned int weaponHash,
		bool createsTrace,
		bool isAccurate);

	extern bulletinstance_t BulletInstance;
	extern bulletinstance_t origBulletInstance;

	extern bool Init(bool true_after_world = false);

	namespace PLAYER
	{
		extern bool Valid();
		extern bool In_Vehicle();

		extern Vector3 Position();
		extern void Set_Position(Vector3 vec);

		extern void Set_Health(float health);
		extern float Get_Health();
		extern void Set_Armor(float armor);
		extern float Get_Armor();
		extern void Godmode(bool value, int type);
		extern float Get_Max_Health();

		extern void Set_Collision(bool v);
		extern void Set_Ragdoll(bool v);
	}

	namespace WEAPON
	{
		extern void NoSpread(bool val);
		extern void NoRecoil(bool val);
		extern void Range(bool val);
		extern void NoReload(bool val);
		extern void BulletInTick();
	}

	extern const char* get_weapon_name(DWORD hash);
}

