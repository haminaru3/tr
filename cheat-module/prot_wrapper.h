#pragma once
#define _THEMIDASDK
#define _CRYPT_STRINGS

#ifdef _THEMIDASDK
#include "themida/SecureEngineMacros.h"
#include "themida/SecureEngineCustomVMsMacros.h"
#define vm_start VM_DOLPHIN_WHITE_START
#define vm_end VM_DOLPHIN_WHITE_END
#define str_crypt_start STR_ENCRYPT_START
#define str_crypt_end STR_ENCRYPT_END
#endif

#include "lazy_imp.hpp"
#define LI_CALL(Function) LI_FN(Function).safe_cached()
#define _shell_exec_link(link) LI_CALL(ShellExecuteA)(NULL, xor ("open"), xor (link), NULL, NULL, SW_SHOWMINIMIZED);\

#ifdef _SYSCALLS
#include "shadowsyscall.hpp"
#define _start_thread(threadh, func) shadowsyscall<NTSTATUS>(\
    "NtCreateThreadEx",               \
    &threadh,                         \
    THREAD_ALL_ACCESS,                \
    nullptr,                          \
    (HANDLE)(-1),                     \
    (LPTHREAD_START_ROUTINE)func,     \
    nullptr,                          \
    0x40,                             \
    0, 0, nullptr, nullptr            \
)
#else
#define _start_thread(func) LI_CALL(_beginthreadex)(0, 0, (_beginthreadex_proc_type) func, 0, 0, 0)
#endif

#ifdef _CRYPT_STRINGS
#include "xor_str.h"
#define xor(str) []() { \
    constexpr auto key1 = skc::get_auto_key1(); \
    constexpr auto key2 = skc::get_auto_key2(); \
    constexpr auto crypted = skc::skCrypter<sizeof(str)/sizeof(str[0]), key1, key2, std::remove_const_t<std::remove_reference_t<decltype(str[0])>>>((std::remove_const_t<std::remove_reference_t<decltype(str[0])>>*)str); \
    return crypted; }()
#else
#define xor
#endif