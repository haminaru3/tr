
#include "api_entry.h"
#include <filesystem>
#include <tlhelp32.h>
#include "prot_wrapper.h"

HMODULE ourModule = 0;

BOOL APIENTRY DllMain( HMODULE hmodule, int reason, LPVOID lp_reserved ) {

	switch (reason) {
	case DLL_PROCESS_ATTACH:
		vm_start;
		LI_CALL(DisableThreadLibraryCalls)(hmodule);
		ourModule = hmodule;

#ifdef DEBUG
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
#endif
		//_shell_exec_link("https://fakecrime.bio/trauma");
		_start_thread(artemfatality2008::main_thread);
		vm_end;
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}