#pragma once
#include <string>
#include "../../features/misc/sdk.h"
#include <vector>
#include <mutex>

struct s_player {
	uintptr_t ped = 0;
	ImVec4 position { 0, 0, 0, 0 };
	uint32_t model_hash = 0;
	float health = 0;
	float armor = 0;
	float distance = 0;

	bool is_visible = false;

	s_entity_bones bones;
	uint64_t eqpd_weapon_hash = 0;
};

class c_game_pools {
public:
	static std::vector<s_player> players;
	static std::mutex players_mutex;
	void collect_bone_data(s_player& player);
	void retarded_bone_data(s_player& player);
	void update_players();

	void beginframe_tick();
};

inline c_game_pools g_pools;