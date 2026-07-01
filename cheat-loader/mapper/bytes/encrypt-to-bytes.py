import os
import sys
import random

def encrypt(data: bytes, key: bytes) -> bytes:
    s = list(range(256))
    j = 0
    for i in range(256):
        j = (j + s[i] + key[i % len(key)]) % 256
        s[i], s[j] = s[j], s[i]

    i = j = 0
    out = bytearray()
    for byte in data:
        i = (i + 1) % 256
        j = (j + s[i]) % 256
        s[i], s[j] = s[j], s[i]
        rnd = s[(s[i] + s[j]) % 256]
        obf = byte ^ rnd ^ ((i * j + 31) % 256)
        out.append(obf)
    return bytes(out)

def to_cpp_array(name: str, data: bytes, indent="    ") -> str:
    array = f"{indent}static inline const unsigned char {name}[] = {{\n"
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        array += indent + "    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n"
    array += f"{indent}}};\n"
    array += f"{indent}static constexpr size_t {name}_size = {len(data)};\n"
    return array

def generate_header(file_stem: str, data: bytes, key: bytes) -> str:
    cls_name = file_stem
    header = f"#pragma once\n\n"
    header += f"#include \"dec_base.h\"\n\n"
    header += f"struct {cls_name} {{\n"
    header += to_cpp_array("data", data)
    header += "\n"
    header += to_cpp_array("key", key)
    header += "\n"
    header += f"    static PBYTE get() {{\n"
    header += f"        static std::vector<BYTE> buf = dec_base::decrypt(data, data_size, key, key_size);\n"
    header += f"        return buf.data();\n"
    header += f"    }}\n"
    header += f"    static size_t size() {{\n"
    header += f"        return data_size;\n"
    header += f"    }}\n"
    header += "};\n"
    return header

def generate_key(length=32):
    return bytes(random.randint(1, 255) for _ in range(length))

def sanitize_name(name):
    return ''.join(c if c.isalnum() else '_' for c in name.lower())

def main():
    if len(sys.argv) < 2:
        print("usage: python encrypt_to_class.py <file1> [file2 ...]")
        return

    for file_path in sys.argv[1:]:
        if not os.path.isfile(file_path):
            print(f"not found: {file_path}")
            continue

        with open(file_path, "rb") as f:
            raw = f.read()

        key = generate_key()
        enc = encrypt(raw, key)
        base = os.path.splitext(os.path.basename(file_path))[0]
        base_sanitized = sanitize_name(base)
        header_file = f"{base_sanitized}.h"

        with open(header_file, "w") as f:
            f.write(generate_header(base_sanitized, enc, key))

        print(f"ok: {header_file}")

if __name__ == "__main__":
    main()
