#pragma once

#include "stdafx.h"

extern PMMVAD(*MiAllocateVad)(UINT_PTR start, UINT_PTR end, LOGICAL deletable);
extern NTSTATUS(*MiInsertVadCharges)(PMMVAD vad, PEPROCESS process);
extern VOID(*MiInsertVad)(PMMVAD vad, PEPROCESS process);

PLDR_DATA_TABLE_ENTRY GetModuleByName(PEPROCESS process, PWCHAR moduleName) {
	UNICODE_STRING moduleNameStr = { 0 };
	KLI_FN(RtlInitUnicodeString)(&moduleNameStr, moduleName);

	PLIST_ENTRY list = &(KLI_FN(PsGetProcessPeb)(process)->Ldr->InLoadOrderModuleList);
	for (PLIST_ENTRY entry = list->Flink; entry != list; ) {
		PLDR_DATA_TABLE_ENTRY module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (KLI_FN(RtlCompareUnicodeString)(&module->BaseDllName, &moduleNameStr, TRUE) == 0) {
			return module;
		}

		entry = module->InLoadOrderLinks.Flink;
	}

	return NULL;
}

NTSTATUS CoreExtend(PREQUEST_EXTEND args) {
	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	KLI_FN(KeAttachProcess)(process);

	PLDR_DATA_TABLE_ENTRY module = GetModuleByName(process, args->Module);
	if (!module) {
		status = STATUS_NOT_FOUND;
		goto cleanup;
	}

	UINT_PTR start = (UINT_PTR)module->DllBase + module->SizeOfImage;
	UINT_PTR end = start + args->Size - 1;

	MEMORY_BASIC_INFORMATION info = { 0 };
	status = KLI_FN(ZwQueryVirtualMemory)(NtCurrentProcess(), (PVOID)start, MemoryBasicInformation, &info, sizeof(info), NULL);
	if (!NT_SUCCESS(status)) {
		goto cleanup;
	}

	if (info.State != MEM_FREE || info.BaseAddress != (PVOID)start || info.RegionSize < args->Size) {
		status = STATUS_INVALID_ADDRESS;
		goto cleanup;
	}

	PMMVAD vad = MiAllocateVad(start, end, TRUE);
	if (!vad) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto cleanup;
	}

	static RTL_OSVERSIONINFOW version = { sizeof(RTL_OSVERSIONINFOW) };
	if (!version.dwBuildNumber) {
		KLI_FN(RtlGetVersion)(&version);
	}

	if (version.dwBuildNumber < 18362) {
		PMMVAD_FLAGS flags = (PMMVAD_FLAGS)&vad->u1.LongFlags;
		flags->Protection = MM_EXECUTE_READWRITE;
		flags->NoChange = 0;
	} else {
		PMMVAD_FLAGS_19H flags = (PMMVAD_FLAGS_19H)&vad->u1.LongFlags;
		flags->Protection = MM_EXECUTE_READWRITE;
		flags->NoChange = 0;
	}

	if (!NT_SUCCESS(status = MiInsertVadCharges(vad, process))) {
		KLI_FN(ExFreePoolWithTag)(vad, 0);
		goto cleanup;
	}

	// We should call MiLockVad but /shrug
	MiInsertVad(vad, process);
	module->SizeOfImage += args->Size;

cleanup:
	KLI_FN(KeDetachProcess)();
	KLI_FN(ObfDereferenceObject)(process);
	return status;
}

NTSTATUS CoreWrite(PREQUEST_WRITE args) {
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > (PVOID)0x00007FFFFFFFFFFFULL) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > (PVOID)0x00007FFFFFFFFFFFULL)) {
		return STATUS_ACCESS_VIOLATION;
	}

	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		SIZE_T outSize = 0;
		status = KLI_FN(MmCopyVirtualMemory)(KLI_FN(IoGetCurrentProcess)(), args->Src, process, args->Dest, (SIZE_T)args->Size, KernelMode, &outSize);
		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}

NTSTATUS CoreRead(PREQUEST_READ args) {
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > (PVOID)0x00007FFFFFFFFFFFULL) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > (PVOID)0x00007FFFFFFFFFFFULL)) {

		return STATUS_ACCESS_VIOLATION;
	}

	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		SIZE_T outSize = 0;
		status = KLI_FN(MmCopyVirtualMemory)(process, args->Src, KLI_FN(IoGetCurrentProcess)(), args->Dest, (SIZE_T)args->Size, KernelMode, &outSize);
		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}

NTSTATUS CoreProtect(PREQUEST_PROTECT args) {
	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		DWORD protect = 0;
		if (SafeCopy(&protect, args->InOutProtect, sizeof(protect))) {
			SIZE_T size = args->Size;

			KLI_FN(KeAttachProcess)(process);
			status = KLI_FN(ZwProtectVirtualMemory)(NtCurrentProcess(), &args->Address, &size, protect, &protect);
			KLI_FN(KeDetachProcess)();

			SafeCopy(args->InOutProtect, &protect, sizeof(protect));
		} else {
			status = STATUS_ACCESS_VIOLATION;
		}
		
		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}

NTSTATUS CoreAlloc(PREQUEST_ALLOC args) {
	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		PVOID address = NULL;
		SIZE_T size = args->Size;

		KLI_FN(KeAttachProcess)(process);
		KLI_FN(ZwAllocateVirtualMemory)(NtCurrentProcess(), &address, 0, &size, MEM_COMMIT | MEM_RESERVE, args->Protect);
		KLI_FN(KeDetachProcess)();

		SafeCopy(args->OutAddress, &address, sizeof(address));

		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}

NTSTATUS CoreFree(PREQUEST_FREE args) {
	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		SIZE_T size = 0;

		KLI_FN(KeAttachProcess)(process);
		KLI_FN(ZwFreeVirtualMemory)(NtCurrentProcess(), &args->Address, &size, MEM_RELEASE);
		KLI_FN(KeDetachProcess)();

		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}

NTSTATUS CoreModule(PREQUEST_MODULE args) {
	PEPROCESS process = NULL;
	NTSTATUS status = KLI_FN(PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		PVOID base = NULL;
		DWORD size = 0;

		KLI_FN(KeAttachProcess)(process);

		PLDR_DATA_TABLE_ENTRY module = GetModuleByName(process, args->Module);
		if (module) {
			base = module->DllBase;
			size = module->SizeOfImage;
		} else {
			status = STATUS_NOT_FOUND;
		}

		KLI_FN(KeDetachProcess)();

		if (NT_SUCCESS(status)) {
			SafeCopy(args->OutAddress, &base, sizeof(base));
			SafeCopy(args->OutSize, &size, sizeof(size));
		}

		KLI_FN(ObfDereferenceObject)(process);
	}

	return status;
}