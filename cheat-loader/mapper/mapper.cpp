#include "mapper.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <conio.h>

//kernel
#include <kdmapper.hpp>
#include <intel_driver.hpp>
#include "bytes/driver_sys.h"

//usermode
#include <comm.h>
#include <hijack.h>
#include <map.h>
#include "bytes/cheat_dll.h"

#include <shellapi.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "../console.hpp"

#define _MSGBOX(x) MessageBox(0, x, "err", MB_OK | MB_ICONERROR | MB_TOPMOST)

void c_mapper::main_thread() {
	vm_lite_start;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (load_kernel()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		inject_dll();
	}
	else {
		MessageBox(0, "failed to load kernel!", "err", MB_OK | MB_ICONERROR | MB_TOPMOST);
	}
	vm_lite_end;
	exit(0);
}

bool c_mapper::load_kernel() {
	vm_hard2_start("load_kernel");

	kdmapper::AllocationMode mode = kdmapper::AllocationMode::AllocatePool;

	console << "loading kmapper";

	if (intel_driver::IsRunning()) {
		_MSGBOX("another instance of driver already loaded!\n\nplease reboot and try again.");
		exit(0);
	}
	
	iqvw64e_device_handle = intel_driver::Load();
	if (iqvw64e_device_handle == INVALID_HANDLE_VALUE) {
		_MSGBOX("kmapper error!");
		console << "kmapper error!";
		return false;
	}

	console << "kmapper loaded";
	console << "loading comm driver";

	NTSTATUS exitCode = 0;
	BYTE* driver_buf = driver_sys::get();
	if (!kdmapper::MapDriver(iqvw64e_device_handle, driver_buf, 0, 0, false, true, mode, false, nullptr, &exitCode)) {
		console << "comm driver loading error! x(";
		_MSGBOX("comm driver loading error!");
		intel_driver::Unload(iqvw64e_device_handle);
		return false;
	}

	console << "comm driver loaded";

	if (!intel_driver::Unload(iqvw64e_device_handle)) {
		console << "<warning> failed to unload kmapper!";
		return false;
	}
	else
		console << "kmapper unloaded";

	volatile BYTE* p = driver_buf;
	for (size_t i = 0; i < driver_sys::size(); ++i) p[i] = 0;
	vm_hard2_end;
	return true;
}

bool c_mapper::inject_dll() {
	vm_hard2_start("inject_dll");
	if (!Comm::Setup()) {
		console << "loader cant establish comm!";
		_MSGBOX("loader cant establish comm!");
		exit(0);
	}
	console << "comm established";

	Comm::Process process(L"GTA5.exe");
	if (!process.Valid()) {
		console << "target process not found!";
		_MSGBOX("start game first!");
		exit(0);
	}
	else {
		console << "target process found";
	}

	//PBYTE dxModuleBase = NULL;
	//NTSTATUS module_check = process.Module(L"d3d11.dll", &dxModuleBase, nullptr);
	//if (module_check != ERROR_SUCCESS || !dxModuleBase)
	//{
	//	console << "game process isnt fully initialized yet";
	//	MessageBox(0, "start loader after game fully initialized!", "retard", MB_OK | MB_ICONERROR | MB_TOPMOST);
	//	exit(0);
	//}
	//else {
	//	console << "process is ready for injection!";
	//} 

	console << "preparing cheat module...";
	auto cheat_buf = cheat_dll::get();

	console << "loading cheat module...";
	PBYTE entry = NULL;
	while (!Map::ExtendMap(process, cheat_buf, entry)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//console << "+";
	}
	console << "module injected!";

	//console << "erasing buffer";
	volatile BYTE* p = cheat_buf;
	for (size_t i = 0; i < cheat_dll::size(); ++i) p[i] = 0;


	console << "executing...";
	if (!Hijack::HijackViaHook(process, entry, "user32.dll", "TranslateMessage")) {
		console << "failed to execute module";
		_MSGBOX("failed to execute module");
		return false;
	}

	console << "";
	console << "anything done, have fun!";
	console << "hint: press key \"delete\" in game to open menu.";
	console << "";
	console << "press any key to exit . . .";
	_getch();
	vm_hard2_end;
	return true;
}