#pragma once
#include <vector>

class dec_base {
public:
    static std::vector<unsigned char> decrypt(const unsigned char* enc, size_t enc_size, const unsigned char* key, size_t key_size);
};
