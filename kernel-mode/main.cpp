#include "stdafx.h"

INT64(NTAPI *EnumerateDebuggingDevicesOriginal)(PVOID, PVOID);

PMMVAD(*MiAllocateVad)(UINT_PTR start, UINT_PTR end, LOGICAL deletable);
NTSTATUS(*MiInsertVadCharges)(PMMVAD vad, PEPROCESS process);
VOID(*MiInsertVad)(PMMVAD vad, PEPROCESS process);


INT64 NTAPI EnumerateDebuggingDevicesHook(PREQUEST_DATA data, PINT64 status) {
	if (KLI_FN(ExGetPreviousMode)() != UserMode || !data) {
		return EnumerateDebuggingDevicesOriginal(data, status);
	}

	REQUEST_DATA safeData = { 0 };
	if (!SafeCopy(&safeData, data, sizeof(safeData)) || safeData.Unique != DATA_UNIQUE) {
		return EnumerateDebuggingDevicesOriginal(data, status);
	}
	
	switch (safeData.Type) {
		HANDLE_REQUEST(Extend, REQUEST_EXTEND);
		HANDLE_REQUEST(Write, REQUEST_WRITE);
		HANDLE_REQUEST(Read, REQUEST_READ);
		HANDLE_REQUEST(Protect, REQUEST_PROTECT);
		HANDLE_REQUEST(Alloc, REQUEST_ALLOC);
		HANDLE_REQUEST(Free, REQUEST_FREE);
		HANDLE_REQUEST(Module, REQUEST_MODULE);
	}

	*status = STATUS_NOT_IMPLEMENTED;
	return 0;
}

NTSTATUS Main() {
    vm_start;
    PCHAR base = (PCHAR)GetKernelBase();
    if (!base) {
        printf("kernel base nf\n");
        return STATUS_FAILED_DRIVER_ENTRY;
    }

    struct {
        const char* name;
        const char* pattern;
        const char* mask;
        PVOID* target;
        const char* alt_pattern;
        const char* alt_mask;
    } patterns[] = {
        {
            "MiAllocateVad",
            "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x30\x48\x8B\xE9\x41\x8B\xF8\xB9\x00\x00\x00\x00\x48\x8B\xF2\x8B\xD1\x41\xB8\x00\x00\x00\x00",
            "xxxx?xxxx?xxxx?xxxxxxxxxxxx????xxxxxxx????",
            (PVOID*)&MiAllocateVad,
            NULL, NULL
        },
        {
            "MiInsertVadCharges",
            "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x20\x8B\x41\x18\x48\x8B\xD9\x44\x0F\xB6\x71\x00\x45\x33\xE4",
            "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxxxxxx?xxx",
            (PVOID*)&MiInsertVadCharges,
            "\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x41\x56\x41\x57\x48\x83\xEC\x40\x44\x0F\xB6\x79\x00\x33\xC0\x44\x0F\xB6\x71\x00\x48\x8B\xEA",
            "xxxx?xxxx?xxxxxxxxxxxxx?xxxxxx?xxx"
        },
        {
            "MiInsertVad",
            "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x20\x8B\x41\x1C\x33\xED\x0F\xB6\x59\x21",
            "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxxxxx",
            (PVOID*)&MiInsertVad,
            "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x20\x8B\x41\x1C",
            "xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxx"
        }
    };

    for (size_t i = 0; i < ARRAYSIZE(patterns); i++) {
        PBYTE addr = (PBYTE)FindPatternImage(base, (PCHAR)patterns[i].pattern, (PCHAR)patterns[i].mask);

        if (!addr && patterns[i].alt_pattern) {
            addr = (PBYTE)FindPatternImage(base, (PCHAR)patterns[i].alt_pattern, (PCHAR)patterns[i].alt_mask);
        }

        if (!addr) {
            printf("%s nf\n", patterns[i].name);
            return STATUS_FAILED_DRIVER_ENTRY;
        }

        *(patterns[i].target) = addr;
    }

    PBYTE addr = (PBYTE)FindPatternImage(base,
        (PCHAR)"\x48\x8B\x05\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xC8\x85\xC0\x78\x40",
        (PCHAR)"xxx????x????xxxxxx");

    if (!addr) {
        printf("xKdEnumerateDebuggingDevices nf\n");
        return STATUS_FAILED_DRIVER_ENTRY;
    }


    *(PVOID*)&EnumerateDebuggingDevicesOriginal = InterlockedExchangePointer(
        (volatile PVOID*)RELATIVE_ADDR(addr, 7),
        (PVOID)EnumerateDebuggingDevicesHook);

    printf("loaded");

    vm_end;
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registryPath) {
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(registryPath);

	return Main();
}