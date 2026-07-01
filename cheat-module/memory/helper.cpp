#include "Helper.h"
#pragma warning(disable : 4996)
#include <Psapi.h>
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#ifdef _WIN64
#define PTRMAXVAL ((uintptr_t)0x000F000000000000)
#else
#define PTRMAXVAL ((uintptr_t)0xFFF00000)
#endif

namespace Helper
{
	bool ValidPTR(uintptr_t ptr)
	{
		if (ptr >= 0x10000 && ptr < PTRMAXVAL)
			return true;

		return false;
	}
}