#pragma once

#include <Windows.h>
#include <process.h>
#include <string>

class c_mapper {
private:
	HANDLE iqvw64e_device_handle;
	bool load_kernel();
	bool inject_dll();

public:
	void main_thread();
};
