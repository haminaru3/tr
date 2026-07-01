#pragma once
#include "../../memory/math.h"
namespace aimbot {
	extern __int64 onBulletInstance(__int64* sBulletInstance_1, const __int64* pWeaponInfo, Vector3* vStart, Vector3* vEnd, float fVelocity, unsigned int weaponHash, bool createsTrace, bool isAccurate);
	extern float screen_distance(float Xx, float Yy, float xX, float yY);
	extern float get_distance(Vector3 to, Vector3 from);
	extern void tick();
}