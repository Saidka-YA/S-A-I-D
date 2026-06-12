#include "crypto_interface.h"
#include <cstring>

static void build_table(ConstBuffer key, uint8_t table[16][16]) {
    bool used[256] = {false};
    uint8_t flat[256];
    size_t pos = 0;
    
    for (size_t i = 0; i < key.size; i++) {
        if (!used[key.data[i]]) {
            used[key.data[i]] = true;
            flat[pos++] = key.data[i];
        }
    }
    
    for (int i = 0; i < 256; i++) {
        if (!used[i]) flat[pos++] = i;
    }
    
    for (int r = 0; r < 16; r++)
        for (int c = 0; c < 16; c++)
            table[r][c] = flat[r * 16 + c];
}

static void find_pos(uint8_t table[16][16], uint8_t val, int& row, int& col) {
    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 16; c++) {
            if (table[r][c] == val) {
                row = r; col = c;
                return;
            }
        }
    }
}

extern "C" {

const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = { "playfair", 16 };
    return &info;
}

size_t get_output_size(size_t input_size, int operation_type) {
    return input_size;
}

int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size < 16) return 1;
    
    uint8_t table[16][16];
    build_table(key, table);
    
    for (size_t i = 0; i < input.size; i += 2) {
        int r1, c1, r2, c2;
        bool has_second = (i + 1 < input.size);
        find_pos(table, input.data[i], r1, c1);
        find_pos(table, has_second ? input.data[i+1] : 0, r2, c2);
        
        if (r1 == r2) {
            output->data[i] = table[r1][(c1 + 1) % 16];
            if (has_second) output->data[i+1] = table[r2][(c2 + 1) % 16];
        } else if (c1 == c2) {
            output->data[i] = table[(r1 + 1) % 16][c1];
            if (has_second) output->data[i+1] = table[(r2 + 1) % 16][c2];
        } else {
            output->data[i] = table[r1][c2];
            if (has_second) output->data[i+1] = table[r2][c1];
        }
    }
    return 0;
}

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size < 16) return 1;
    
    uint8_t table[16][16];
    build_table(key, table);
    
    for (size_t i = 0; i < input.size; i += 2) {
        int r1, c1, r2, c2;
        bool has_second = (i + 1 < input.size);
        find_pos(table, input.data[i], r1, c1);
        find_pos(table, has_second ? input.data[i+1] : 0, r2, c2);
        
        if (r1 == r2) {
            output->data[i] = table[r1][(c1 + 15) % 16];
            if (has_second) output->data[i+1] = table[r2][(c2 + 15) % 16];
        } else if (c1 == c2) {
            output->data[i] = table[(r1 + 15) % 16][c1];
            if (has_second) output->data[i+1] = table[(r2 + 15) % 16][c2];
        } else {
            output->data[i] = table[r1][c2];
            if (has_second) output->data[i+1] = table[r2][c1];
        }
    }
    return 0;
}

}