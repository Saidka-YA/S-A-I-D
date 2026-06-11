#include "crypto_interface.h"

static uint8_t mod_inverse(uint8_t a) {
    for (int i = 1; i < 256; i++) {
        if ((a * i) % 256 == 1) return i;
    }
    return 1;
}

extern "C" {

const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = { "affine", 2 };
    return &info;
}

size_t get_output_size(size_t input_size, int operation_type) {
    return input_size;
}

int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size < 2) return 1;
    if (key.data[0] % 2 == 0) return 2;
    
    uint8_t a = key.data[0];
    uint8_t b = key.data[1];
    
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = (input.data[i] * a + b) % 256;
    }
    return 0;
}

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size < 2) return 1;
    
    uint8_t a = key.data[0];
    uint8_t b = key.data[1];
    uint8_t a_inv = mod_inverse(a);
    
    for (size_t i = 0; i < input.size; ++i) {
        int val = input.data[i] - b;
        if (val < 0) val += 256;
        output->data[i] = (val * a_inv) % 256;
    }
    return 0;
}

}