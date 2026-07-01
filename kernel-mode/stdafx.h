#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntimage.h>
#include <minwindef.h>

#include <intrin.h>
#include <cstdint>

#include "vlizer/VirtualizerSDKMacros.h"
#include "vlizer/VirtualizerSDKCustomVMsMacros.h"
#define vm_start VIRTUALIZER_DOLPHIN_WHITE_START
#define vm_end VIRTUALIZER_DOLPHIN_WHITE_END


//#define printf(fmt, ...) DbgPrintEx(0, 0, "[dbg] "fmt, ##__VA_ARGS__)
#define printf

#define HANDLE_REQUEST(name, args) \
    case RequestType##name: {                                     \
        args safe = { 0 };                                        \
        if (!SafeCopy(&safe, safeData.Arguments, sizeof(args))) { \
            *status = STATUS_ACCESS_VIOLATION;                    \
            return 0;                                             \
        }                                                         \
        *status = Core##name(&safe);                              \
        return 0;                                                 \
    }

#include "kli.hpp"
#include "core.h"
#include "util.h"