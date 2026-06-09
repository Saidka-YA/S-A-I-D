#ifndef CRYPTO_INTERFACE_H
#define CRYPTO_INTERFACE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ConstBuffer {
    const uint8_t* data;
    size_t size;
};

struct MutBuffer {
    uint8_t* data;
    size_t size;
};

struct AlgorithmInfo {
    const char* algorithm_name;
    size_t key_size;
};

#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
inline void secure_clear_memory(void* v, size_t n) {
    volatile uint8_t* p = static_cast<volatile uint8_t*>(v);
    while (n--) *p++ = 0;
}
#endif