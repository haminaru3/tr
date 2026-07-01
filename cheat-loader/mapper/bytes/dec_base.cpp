#include "dec_base.h"
#include "../protect/prot_defines.h"

std::vector<unsigned char> dec_base::decrypt(const unsigned char* enc, size_t enc_size, const unsigned char* key, size_t key_size) {
    vm_hard2_start("dec_bytes");
    unsigned char s[256];
    for (int i = 0; i < 256; ++i) s[i] = i;
    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + s[i] + key[i % key_size]) % 256;
        std::swap(s[i], s[j]);
    }
    int i = 0;
    j = 0;
    std::vector<unsigned char> out(enc_size);
    for (size_t n = 0; n < enc_size; ++n) {
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;
        std::swap(s[i], s[j]);
        unsigned char rnd = s[(s[i] + s[j]) % 256];
        out[n] = enc[n] ^ rnd ^ ((i * j + 31) % 256);
    }
    vm_hard2_end;
    return out;
}
