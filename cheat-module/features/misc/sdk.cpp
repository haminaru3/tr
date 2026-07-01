#include "sdk.h"
#include <string>
#include <float.h>
#include "../../vladkatafalk2006.h"
#include "../../prot_wrapper.h"
#include "../../memory/c_memory_mgmt.hpp"

namespace SDK
{
	typedef BOOLEAN(__cdecl* worldtoscreen_t)(Vector3* WorldPos, float* x, float* y);
	typedef __int64(__fastcall* bulletinstance_t)(
		__int64* sBulletInstance_1,
		const __int64* pWeaponInfo,
		Vector3* vStart,
		Vector3* vEnd,
		float fVelocity,
		unsigned int weaponHash,
		bool createsTrace,
		bool isAccurate);

	bulletinstance_t BulletInstance = nullptr;
	bulletinstance_t origBulletInstance = nullptr;


#pragma pack(push, 1)
	class c_viewport {
	public:
		char _0x0000[0x24C];
		float matrix[0x10];
	};//Size=0x028C
#pragma pack(pop)
	c_viewport* game_viewport = nullptr;

	bool WorldToScreen(Vector3 pos, ImVec2* out) {
		if (!_is_valid_ptr(game_viewport)) return false;
		Vector3	tmp;

		tmp.x = (game_viewport->matrix[1] * pos.x) + (game_viewport->matrix[5] * pos.y) + (game_viewport->matrix[9] * pos.z) + game_viewport->matrix[13];
		tmp.y = (game_viewport->matrix[2] * pos.x) + (game_viewport->matrix[6] * pos.y) + (game_viewport->matrix[10] * pos.z) + game_viewport->matrix[14];
		tmp.z = (game_viewport->matrix[3] * pos.x) + (game_viewport->matrix[7] * pos.y) + (game_viewport->matrix[11] * pos.z) + game_viewport->matrix[15];

		if (tmp.z < 0.1f)
			return false;

		tmp.z = 1.0f / tmp.z;

		tmp.x *= tmp.z;
		tmp.y *= tmp.z;

		int w = ImGui::GetIO().DisplaySize.x;
		int h = ImGui::GetIO().DisplaySize.y;

		int x = ((w / 2.f) + (.5f * tmp.x * w));
		int y = ((h / 2.f) - (.5f * tmp.y * h));

		if (x == 0.f || y == 0.f) return false;
		out->x = x;
		out->y = y;

		return true;

	}

	typedef void* (__fastcall* get_bone_pos_mask_t)(uintptr_t, __m128*, unsigned int);
	get_bone_pos_mask_t get_bone_pos;

	ImVec2 GetBonePosW2S(uintptr_t cPed, int32_t wMask)
	{
		__m128 pos;
		get_bone_pos(cPed, &pos, wMask);
		ImVec2 tmp;
		WorldToScreen(Vector3::FromM128(pos), &tmp);

		return tmp;
	}

	Vector3 GetBonePosVec3(uintptr_t ped, const int32_t bone_mask) {
		__m128 pos;
		get_bone_pos(ped, &pos, bone_mask);
		return Vector3::FromM128(pos);
	}

	Vector3 GetBonePosVec3(uintptr_t ped, e_bones bone) {
		return GetBonePosVec3(ped, (WORD)bone);
	}

	typedef struct {
		int result;
		bool did_hit_anything;
		bool did_hit_entity;
		int32_t hit_entity;
		PVector3 hit_coords;
		PVector3 hit_normal;
	} s_raycast_result;

	struct s_raycast {
		enum e_raycast_flags {
			IntersectEverything = -1,
			IntersectMap = 1,
			IntersectVehicles = 2,
			IntersectPeds1 = 4,
			IntersectPeds2 = 8,
			IntersectObjects = 16,
			IntersectVegetation = 256
		};

		typedef DWORD(__cdecl* start_shape_test_capsule_t)(PVector3 start_p, PVector3 end_p, float radius, e_raycast_flags flags, DWORD entity, int p9);
		start_shape_test_capsule_t start_shape_test_capsule = nullptr;

		typedef int(__cdecl* get_raycast_result_t)(DWORD handle, bool* hit, PVector3* end_p, PVector3* normal_surface, int32_t* hit_entity);
		get_raycast_result_t get_raycast_result = nullptr;

		DWORD default_flags = IntersectMap | IntersectVehicles | IntersectObjects;
	}; inline s_raycast g_raycast;

	bool RaycastTest(Vector3 start_p, Vector3 end_p) {
		PVector3 nullvec(0, 0, 0), hitcordmap(0, 0, 0);
		s_raycast_result raycast_res_map;

		PVector3 start_pos(start_p.x, start_p.y, start_p.z);
		PVector3 end_pos(end_p.x, end_p.y, end_p.z);

		auto _test_capsule = g_raycast.start_shape_test_capsule(start_pos, end_pos, 0, (s_raycast::e_raycast_flags)g_raycast.default_flags, (DWORD)0, 7);
		if (_test_capsule) {
			g_raycast.get_raycast_result(_test_capsule, &raycast_res_map.did_hit_anything, &hitcordmap, &nullvec, &raycast_res_map.hit_entity);

			if (raycast_res_map.did_hit_anything) {
				return false;
			}
			return true;
		}
	}

	uintptr_t CameraViewAngles;
	uintptr_t GetCameraViewAngles()
	{
		if (Helper::ValidPTR(CameraViewAngles))
			return *(uintptr_t*)(CameraViewAngles + 0x0);
	}

	uintptr_t ReplayInterface;
	uintptr_t GetReplayInterface()
	{
		if (Helper::ValidPTR(ReplayInterface))
			return *(uintptr_t*)(ReplayInterface + 0x0);
	}

	int nnndnsf; // <--- what the fuck is this

	uintptr_t World;
	uintptr_t LocalPlayer;
	uintptr_t PlayerInfo;
	uintptr_t CurWeapon;

	bool Init(bool true_after_world)
	{
		str_crypt_start;
		const char* process = "GTA5.exe";

		World = (uintptr_t)g_mem_mgmt.fix_mov(g_mem_mgmt.find_ptr(process, patterns::world));
		World = *(uintptr_t*)(World + 0x0);
		if (World == NULL) {
			log_smth("world null");
			return false;
		}

		if (true_after_world) {
			log_smth("world intialized");
			return true;
		}

		LocalPlayer = *(uintptr_t*)(World + 0x08);
		if (LocalPlayer == NULL) {
			log_smth("lplayer null");
			return false;
		}

		CameraViewAngles = (uintptr_t)g_mem_mgmt.fix_mov(g_mem_mgmt.find_ptr(process, patterns::cameraviewangles));
		if (CameraViewAngles == NULL) {
			log_smth("cviewangles null");
			return false;
		}
		ReplayInterface = (uintptr_t)g_mem_mgmt.fix_mov(g_mem_mgmt.find_ptr(process, patterns::replayinterface));
		if (ReplayInterface == NULL) {
			log_smth("replayinterface null");
			return false;
		}

		game_viewport = *(c_viewport**)g_mem_mgmt.fix_mov(g_mem_mgmt.find_ptr(process, patterns::gameviewport));
		if (game_viewport == NULL) {
			log_smth("viewp null");
		}

		BulletInstance = reinterpret_cast<bulletinstance_t>(g_mem_mgmt.find_ptr(process, patterns::bulletinstance));
		if (BulletInstance == NULL) {
			log_smth("bi null");
			MessageBoxA(0, "silent aimbot is unavailable...", "fuck", MB_OK | MB_ICONWARNING);
		}

		get_bone_pos = reinterpret_cast<get_bone_pos_mask_t>(g_mem_mgmt.find_ptr(process, patterns::getentitybonepos));
		if (get_bone_pos == NULL) {
			log_smth("get_bone_pos null");
			return false;
		}

		g_raycast.start_shape_test_capsule = (s_raycast::start_shape_test_capsule_t)(
			g_mem_mgmt.find_ptr(process, patterns::raycast_startshapetestcapsule) - 0xBE);
		if (g_raycast.start_shape_test_capsule == NULL) {
			log_smth("start_shape_test_capsule null");
			return false;
		}

		g_raycast.get_raycast_result = (s_raycast::get_raycast_result_t)(g_mem_mgmt.find_ptr(process, patterns::raycast_getraycastresult));
		if (g_raycast.get_raycast_result == NULL) {
			log_smth("get_raycast_result null");
			return false;
		}

		str_crypt_end;
		return true;

	}

	namespace PLAYER
	{
		bool Valid()
		{
			if (Helper::ValidPTR(World))
			{
				LocalPlayer = *(uintptr_t*)(World + 0x8);
				if (LocalPlayer != NULL)
					return true;
			}
			return false;
		}
		bool In_Vehicle()
		{
			if (PLAYER::Valid())
			{
				BYTE state = *(BYTE*)(LocalPlayer + 0x148B);
				if (state == 12)
					return true;
			}
			return false;
		}
		
		Vector3 Position()
		{
			if (PLAYER::Valid())
				return *(Vector3*)(LocalPlayer + 0x90);
		}

		void Set_Position(Vector3 vec)
		{
			*(Vector3*)(LocalPlayer + 0x90) = vec;

			//if (VEHICLE::Valid())
			//{
			//	uintptr_t veh = *(uintptr_t*)(LocalPlayer + 0xD10);
			//	uintptr_t pos = *(uintptr_t*)(veh + 0x30);
			//	*(Vector3*)(pos + 0x50) = vec;
			//	*(Vector3*)(veh + 0x90) = vec;
			//}
		}

		void Set_Health(float health)
		{
			if (PLAYER::Valid()) {
				*(float*)(LocalPlayer + 0x280) = health;

			}
		}

		float Get_Max_Health()
		{
			if (PLAYER::Valid())
				return *(float*)(LocalPlayer + 0x284);
		}

		void SetMaxHealth()
		{
			if (PLAYER::Valid()) {
				*(float*)(LocalPlayer + 0x280) = Get_Max_Health();
			}
		}

		void Godmode(bool value, int type)
		{
			if (type == 0) { // autofill type (+10 social credit, best var for all servers)
				if (value && PLAYER::Get_Health() < Get_Max_Health())
					SetMaxHealth();
			}

			if (type == 1) { // flt_max health (2billions+-)
				Set_Health(FLT_MAX);
			}

			if (type == 2) { // def gm via one offset (fixme)
				if (value)
					*(float*)(LocalPlayer + 0x189) = true;
				if (!value)
					*(float*)(LocalPlayer + 0x189) = false;
			}
		}
	

		float Get_Health()
		{
			if (PLAYER::Valid())
				return *(float*)(LocalPlayer + 0x280);
		}


		void Set_Armor(float armor)
		{
			if (PLAYER::Valid())
				*(float*)(LocalPlayer + 0x150C) = armor;
		}

		float Get_Armor()
		{
			if (PLAYER::Valid())
				return *(float*)(LocalPlayer + 0x150C);
		}

		void Set_Collision(bool v) {
			if (PLAYER::Valid()) {
				uintptr_t* value = *(uintptr_t**)((uintptr_t)LocalPlayer + 0x30);
				if (!_is_valid_ptr(value)) return;
				uintptr_t* value1 = *(uintptr_t**)((uintptr_t)value + 0x10);
				if (!_is_valid_ptr(value1)) return;
				uintptr_t* value2 = *(uintptr_t**)((uintptr_t)value1 + 0x20);
				if (!_is_valid_ptr(value2)) return;
				uintptr_t* value3 = *(uintptr_t**)((uintptr_t)value2 + 0x70);
				if (!_is_valid_ptr(value3)) return;
				uintptr_t* value4 = *(uintptr_t**)((uintptr_t)value3 + 0x0);
				if (!_is_valid_ptr(value4)) return;

				if (v)
					*(float*)((uintptr_t)value4 + 0x2C) = -1.f;
				else 
					*(float*)((uintptr_t)value4 + 0x2C) = 0.25f;
			}
		}

		void Set_Ragdoll(bool v) {
			if (PLAYER::Valid()) {
				if (v)
					*(BYTE*)(LocalPlayer + 0x1098) = 0x1;
				else
					*(BYTE*)(LocalPlayer + 0x1098) = 0x20;
			}
		}

		Vector3 SetVelocity()
		{
			return *(Vector3*)(SDK::LocalPlayer + 0x320) = Vector3(0, 0, 0);
		}

		uintptr_t ObjectNav()
		{
			if (SDK::PLAYER::Valid())
				return *(uintptr_t*)(SDK::LocalPlayer + 0x30);
		}

		Vector3 NavCords(Vector3 Cords)
		{
			if (!ObjectNav) return Vector3{ 0,0,0 };
			return *(Vector3*)(ObjectNav() + 0x50) = Cords;
		}

		Vector4 NavRotation(Vector4 Coords)
		{
			return *(Vector4*)(ObjectNav() + 0x30) = Coords;
		}

	}

	namespace WEAPON
	{
		void NoSpread(bool val)
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						if (val)
						{
							*(float*)(Addr + 0x84) = 0.f;
							*(float*)(Addr + 0x0124) = 0.0f;
						}
						else
						{
							float val = *(float*)(Addr + 0x84);
							if (val == 0.0f)
								*(float*)(Addr + 0x84) = 1.0f;
								*(float*)(Addr + 0x0124) = 1.0f;
						}
					}
				}
			}
		}
		void NoRecoil(bool val)
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						if (val)
						{
							*(float*)(Addr + 0x2F4) = 0.0f;
						}
						else
						{
							float val = *(float*)(Addr + 0x2F4);
							if (val == 0.0f)
								*(float*)(Addr + 0x2F4) = 0.3f;
						}
					}
				}
			}
		}

		void Range(bool val)
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						if (val)
						{
							*(float*)(Addr + 0x028C) = 99999.f;
						}
						else
						{
							float val = *(float*)(Addr + 0x028C);
							if (val == 9999.0f)
								*(float*)(Addr + 0x028C) = 130.f;
						}
					}
				}
			}
		}

		void NoReload(bool val)
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						if (val)
						{
							*(float*)(Addr + 0x0128) = 99999.f;
							*(float*)(Addr + 0x012C) = 99999.f;
							*(float*)(Addr + 0x0130) = 99999.f;
							*(float*)(Addr + 0x0134) = 99999.f;
						}
						else
						{
							*(float*)(Addr + 0x0128) = 1.f;
							*(float*)(Addr + 0x012C) = 1.f;
							*(float*)(Addr + 0x0130) = 1.f;
							*(float*)(Addr + 0x0134) = 1.f;
						}
					}
				}
			}
		}


		void BulletInTick()
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						// bullet speed
						*(float*)(Addr + 0x011C) = FLT_MAX;
						// bullet force
						*(float*)(Addr + 0x00D8) = FLT_MAX;
					}
				}
			}
		}

		void RapidFire()
		{
			if (PLAYER::Valid())
			{
				uintptr_t Addr = *(uintptr_t*)(LocalPlayer + 0x10B8);
				if (Helper::ValidPTR(Addr))
				{
					Addr = *(uintptr_t*)(Addr + 0x20);
					if (Helper::ValidPTR(Addr))
					{
						// time between shots
						*(float*)(Addr + 0x013C) = FLT_MIN;

						*(int32_t*)(Addr + 0x0138) = INT_MAX;
					}
				}
			}
		}
	}

	const char* get_weapon_name(DWORD hash)
	{
		struct Weapon {
			DWORD hash;
			const char* name;
		};

		static const Weapon weapons[] = {
			{0x92A27487, "dagger"},
			{0x958A4A8F, "bat"},
			{0xF9E6AA4B, "bottle"},
			{0x84BD7BFD, "crow_bar"},
			{0xA2719263, ""},
			{0x8BB05FD7, "flashlight"},
			{0x440E4788, "golf"},
			{0x4E875F73, "hammer"},
			{0xF9DCBF2D, "hatchet"},
			{0xD8DF3C3C, "knuckle"},
			{0x99B507EA, "knife"},
			{0xDD5DF8D9, "machete"},
			{0xDFE37640, "switch_blade"},
			{0x678B81B1, "night_stick"},
			{0x19044EE0, "wrench"},
			{0xCD274149, "axe"},
			{0x94117305, "pool_cue"},
			{0x1B06D571, "pistol"},
			{0xBFE256D4, "pistol_mkII"},
			{0x5EF9FEC4, "combat_pistol"},
			{0x22D8FE39, "ap_pistol"},
			{0x3656C8C1, "tazer"},
			{0x99AEEB3B, "pistol_50"},
			{0xBFD21232, "sns_pistol"},
			{0x88374054, "sns_pistol_mkII"},
			{0xD205520E, "heavy pistol"},
			{0x83839C4,	 "vintage_pisol"},
			{0x47757124, "flare"},
			{0xDC4DB296, "marksman_pistol"},
			{0xC1B3C3D1, "revolver"},
			{0xCB96392F, "revolver_mkII"},
			{0x97EA20B8, "d.a_revolver"},
			{0x13532244, "micro_smg"},
			{0x2BE6766B, "smg"},
			{0x78A97CD0, "smg_mkII"},
			{0xEFE7E2DF, "assault_smg"},
			{0xA3D4D34,  "pdw"},
			{0xDB1AA450, "tec_9"},
			{0xBD248B55, "mini_smg"},
			{0x1D073A89, "pump_shotgun"},
			{0x555AF99A, "pump_shotgun_mkII"},
			{0x7846A318, "sawnoff"},
			{0xE284C527, "assault_shotgun"},
			{0x9D61E50F, "bullpup_shotgun"},
			{0xA89CB99E, "musket"},
			{0x3AABBBAA, "heavy_shotgun"},
			{0xEF951FBB, "db_shotgun"},
			{0x12E82D3D, "auto_shotgun"},
			{0xBFEFFF6D, "assault_rifle"},
			{0x394F415C, "assault_rifle_mkII"},
			{0x83BF0278, "carbine"},
			{0xFAD1F1C9, "carbine_mkII"},
			{0xAF113F99, "advanced_rifle"},
			{0xC0A3098D, "special_carbine"},
			{0x969C3D67, "special_carbine_mkII"},
			{0x7F229F94, "bullpup_rifle"},
			{0x84D6FAFD, "bullpup_rifle_mkII"},
			{0x624FE830, "compact_rifle"},
			{0x9D07F764, "machine_gun"},
			{0x7FD62962, "combat_mg"},
			{0xDBBD7280, "combat_mg_mkII"},
			{0x61012683, "gusenberg"},
			{0x5FC3C11, "sniper_rifle"},
			{0xC472FE2, "heavy_sniper"},
			{0xA914799, "heavy_sniper_mkII"},
			{0xC734385A, "marksman_rifle"},
			{0x6A6C02E0, "marksman_rifle_mkII"},
			{0xB1CA77B1, "RPG"},
			{0xA284510B, "nade_launcher"},
			{0x4DD2DC56, "smokelauncher"},
			{0x42BF8A85, "minigun"},
			{0x7F7497E5, "firework"},
			{0x6D544C99, "railgun"},
			{0x63AB0442, "homing_launcher"},
			{0x781FE4A, "compact_launcher"},
			{0x93E220BD, "nade"},
			{0xA0973D5E, "bzgas"},
			{0xFDBC8A50, "smoke"},
			{0x497FACC3, "flare"},
			{0x24B17070, "molotov"},
			{0x2C3731D9, "sticky_bomb"},
			{0xAB564B93, "prox"},
			{0x787F0BB, "snow"},
			{0xBA45E8B8, "pipe_bomb"},
			{0x23C9F95C, "ball"},
			{0x34A67B97, "petrol"},
			{0x60EC506, "fire_ext"},
			{0xFBAB5776, "parachute"},
		};

		for (const auto& weapon : weapons) {
			if (weapon.hash == hash)
				return weapon.name;
		}

		return "";
	}
}