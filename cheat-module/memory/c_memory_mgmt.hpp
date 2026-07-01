#pragma once
#include <iostream>
#include <windows.h>
#include <string>

class c_memory_mgmt {
public:
	uint8_t* find_ptr(const std::string_view module, const std::string_view signature);

	template<typename T = uint8_t*>
	inline T fix_mov(uint8_t* patternMatch) {
		return reinterpret_cast<T>(patternMatch + *reinterpret_cast<int32_t*>(patternMatch + 3) + 7);
	}
};

inline c_memory_mgmt g_mem_mgmt;