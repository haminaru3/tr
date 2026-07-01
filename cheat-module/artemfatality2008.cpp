#include "artemfatality2008.hpp"

#include "features/misc/sdk.h"
#include "game/pools/game_pools.hpp"
#include "features/aimbot/aimbot.h"

#include "prot_wrapper.h"

#include <thread>
#include <chrono>

#include "d3d11hook.hpp"
#include "minhook/include/MinHook.h"

namespace artemfatality2008 {
	DWORD WINAPI aimbot_thread(LPVOID) {
		while (!variables::menu::unhook) {
			aimbot::tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		return 0;
	}

	DWORD WINAPI main_thread(HMODULE hmod) {

		SDK::Init(false);
		d3d11hook::init();

		if (SDK::BulletInstance) {
			MH_CreateHook(SDK::BulletInstance, aimbot::onBulletInstance, reinterpret_cast<void**>(&SDK::origBulletInstance));
			if (MH_EnableHook(SDK::BulletInstance) != MH_OK) {
				log_smth("error 🐖 hooking");
			}
		}

		_start_thread(aimbot_thread);

		while (!variables::menu::unhook) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		d3d11hook::release();

		LI_CALL(FreeLibraryAndExitThread)(hmod, 0);
		return TRUE;

	}
}