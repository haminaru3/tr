#pragma once
#include <type_traits>

namespace skc
{
    constexpr uint32_t fnv1a_hash(const char* str, uint32_t hash = 0x811C9DC5) {
        return *str ? fnv1a_hash(str + 1, (*str ^ hash) * 0x01000193) : hash;
    }

    constexpr uint8_t get_auto_key1() {
        return (fnv1a_hash(__DATE__) ^ fnv1a_hash(__TIME__)) & 0xFF;
    }
    constexpr uint8_t get_auto_key2() {
        return (fnv1a_hash(__TIME__) ^ 0x55AA00FF) & 0xFF;
    }

    template <int _size, char _key1, char _key2, typename T>
    class skCrypter
    {
    public:
        __forceinline constexpr skCrypter(T* data) {
            crypt(data);
        }

        __forceinline T* get() { return _storage; }
        __forceinline int size() { return _size; }

        __forceinline T* decrypt() {
            if (isEncrypted()) crypt(_storage);
            return _storage;
        }

        __forceinline operator T* () { return decrypt(); }

    private:
        __forceinline constexpr void crypt(T* data) {
            for (int i = 0; i < _size; i++) {
                _storage[i] = data[i] ^ (_key1 + i % (1 + _key2));
            }
        }

        __forceinline bool isEncrypted() {
            return _storage[_size - 1] != 0;
        }

        T _storage[_size]{};
    };
}