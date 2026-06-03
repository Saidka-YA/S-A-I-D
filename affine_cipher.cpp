#include "crypto_interface.h"

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
    
    uint8_t a = key.data[0];
    uint8_t b = key.data[1];
    
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = (input.data[i] * a + b) % 256;
    }
    return 0;
}

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    return encrypt(key, input, output);
}

}