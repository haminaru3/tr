#pragma once

#ifndef NO_PROT
#include "codevirt/VirtualizerSDKMacros.h"
#include "codevirt/VirtualizerSDKCustomVMsMacros.h"
#include "vmp/include/VMProtectSDK.h"

#define vm_lite_start //VIRTUALIZER_TIGER_WHITE_START
#define vm_lite_end //VIRTUALIZER_TIGER_WHITE_END
#define vm_hard_start //VIRTUALIZER_LION_WHITE_START
#define vm_hard_end //VIRTUALIZER_LION_WHITE_END
#define vm_hard2_start(name) VMProtectBegin(name)
#define vm_hard2_end VMProtectEnd()

#define str_crypt_start //VIRTUALIZER_STR_ENCRYPT_START
#define str_crypt_end //VIRTUALIZER_STR_ENCRYPT_END
#define wstr_crypt_start //VIRTUALIZER_STR_ENCRYPTW_START
#define wstr_crypt_end //VIRTUALIZER_STR_ENCRYPTW_END

#define _str_crypt(v) VMProtectDecryptStringA(v)
#define _wstr_crypt(v) VMProtectDecryptStringW(v)
#else
#define vm_lite_start
#define vm_lite_end
#define vm_hard_start
#define vm_hard_end
#define vm_hard2_start(x)
#define vm_hard2_end

#define str_crypt_start
#define str_crypt_end
#define wstr_crypt_start
#define wstr_crypt_end

#define _str_crypt
#define _wstr_crypt
#endif
