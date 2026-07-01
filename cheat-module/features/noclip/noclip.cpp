#include "noclip.hpp"
#include "../../variables/variables.hpp"
#include "../../features/misc/sdk.h"


namespace noclip {
	Vector3 reset_velocity() {
		return *(Vector3*)(SDK::LocalPlayer + 0x320) = Vector3(0, 0, 0);
	}

	uintptr_t obj_nav() {
		if (SDK::PLAYER::Valid())
			return *(uintptr_t*)(SDK::LocalPlayer + 0x30);
	}

	Vector3 nav_coords(Vector3 cords) {
		if (!obj_nav) return Vector3{ 0,0,0 };
		return *(Vector3*)(obj_nav() + 0x50) = cords;
	}

	Vector4 nav_rot(Vector4 coords) {
		return *(Vector4*)(obj_nav() + 0x30) = coords;
	}

	bool freeze(bool toggle) {

		if (!SDK::LocalPlayer) return 0;
		uintptr_t player = *(uintptr_t*)(SDK::LocalPlayer + 0x10A8);
		if (toggle)
			return *(BYTE*)(player + 0x238) = 2;
		if (!toggle)
			return *(BYTE*)(player + 0x238) = 0;
	}

	void tick() {
		if (variables::player::noclip)
		{
			float noclipspeed = 0.1f + (variables::player::noclipspeed * 0.099f);
			
			if (variables::player::noclipboost && (GetAsyncKeyState(variables::player::noclipboostbind) & 0x8000)) {
				noclipspeed *= 2;
			}


            if (variables::player::noclip_state) {
                nav_rot(Vector4{ 0, 0, 0, 0 });

                Vector3 actualPos = SDK::PLAYER::Position();
                Vector3 newPos = actualPos;
                bool positionChanged = false;

                auto handleMovement = [&](int key, const std::function<Vector3(Vector3, Vector3)>& movementFunc) {
                    if (GetAsyncKeyState(key) & 0x8000) {
                        uintptr_t addr = SDK::GetCameraViewAngles();
                        Vector3 angles = *(Vector3*)(addr + 0x3D0);

                        if (angles.x == 0 && angles.y == 0 && angles.z == 0) {
                            angles = *(Vector3*)(addr + 0x40);
                        }

                        newPos = movementFunc(newPos, angles);
                        positionChanged = true;

                        if (key != variables::player::noclipleft && key != variables::player::noclipright) {
                            nav_rot(Vector4{ -angles.x, -angles.y, -angles.z, 0 });
                        }
                    }
                    };

                handleMovement(variables::player::noclipforward, [noclipspeed](Vector3 pos, Vector3 angles) {
                    return Vector3{
                        pos.x + angles.x * noclipspeed,
                        pos.y + angles.y * noclipspeed,
                        pos.z + angles.z * noclipspeed
                    };
                    });

                handleMovement(variables::player::noclipbackward, [noclipspeed](Vector3 pos, Vector3 angles) {
                    return Vector3{
                        pos.x - angles.x * noclipspeed,
                        pos.y - angles.y * noclipspeed,
                        pos.z - angles.z * noclipspeed
                    };
                    });

                handleMovement(variables::player::noclipleft, [noclipspeed](Vector3 pos, Vector3 angles) {
                    return Vector3{
                        pos.x - angles.y * noclipspeed,
                        pos.y + angles.x * noclipspeed,
                        pos.z
                    };
                    });

                handleMovement(variables::player::noclipright, [noclipspeed](Vector3 pos, Vector3 angles) {
                    return Vector3{
                        pos.x + angles.y * noclipspeed,
                        pos.y - angles.x * noclipspeed,
                        pos.z
                    };
                    });

                handleMovement(variables::player::noclipup, [noclipspeed](Vector3 pos, Vector3) {
                    return Vector3{
                        pos.x,
                        pos.y,
                        pos.z + noclipspeed
                    };
                    });

                handleMovement(variables::player::noclipdown, [noclipspeed](Vector3 pos, Vector3) {
                    return Vector3{
                        pos.x,
                        pos.y,
                        pos.z - noclipspeed
                    };
                    });

                if (positionChanged) {
                    reset_velocity();
                    freeze(true);
                    SDK::PLAYER::Set_Position(newPos);
                    nav_coords(newPos);
                    freeze(false);
                }
            }
		}
	}
}



