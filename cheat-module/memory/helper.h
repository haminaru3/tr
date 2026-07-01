#include <cstdlib>
#include <Windows.h>
#include <iostream>
#include <vector>

#define _is_valid_ptr(x) (x != NULL && \
						(DWORD_PTR)x >= 0x10000 && \
						(DWORD_PTR)x < 0x00007FFFFFFEFFFF)

namespace Helper
{
	extern bool ValidPTR(uintptr_t ptr);
}