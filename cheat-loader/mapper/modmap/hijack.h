#pragma once

namespace Hijack {
	BOOLEAN HijackViaHook(Comm::Process &process, PVOID entry, LPCSTR moduleName, LPCSTR functionName);
}