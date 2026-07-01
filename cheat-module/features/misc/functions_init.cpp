#include "sdk.h"

namespace _hacks
{
	void player_tick()
	{
		SDK::PLAYER::Godmode(variables::player::godmode, variables::player::godmodetype);

		static bool is_col_set = false, is_rg_set = false;

		if (variables::player::nocollision) {
			SDK::PLAYER::Set_Collision(true);
			is_col_set = true;
		}
		else if (!variables::player::nocollision && is_col_set) {
			SDK::PLAYER::Set_Collision(false);
			is_col_set = false;
		}


		if (variables::player::noragdoll || variables::player::noclip_state) {
			SDK::PLAYER::Set_Ragdoll(true);
			is_rg_set = true;
		}
		else if ((!variables::player::noragdoll || !variables::player::noclip_state) && is_rg_set) {
			SDK::PLAYER::Set_Ragdoll(false);
			is_rg_set = false;
		}

		if (variables::player::noclip)
			noclip::tick();
	}

	void weapon_tick()
	{
		SDK::WEAPON::NoRecoil(variables::weapon::norecoil);
		SDK::WEAPON::NoSpread(variables::weapon::nospread);
		SDK::WEAPON::Range(variables::weapon::range);

		SDK::WEAPON::NoReload(variables::weapon::noreload);
		SDK::WEAPON::BulletInTick();
	}

	void tick()
	{
		player_tick();
		weapon_tick();
	}
}