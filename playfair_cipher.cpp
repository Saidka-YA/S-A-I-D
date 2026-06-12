#include "crypto_interface.h"
#include <cstring>
#include <vector>

using namespace std;

static void init_table(ConstBuffer key, uint8_t table[16][16]) {
    uint8_t used[256] = {0};
    uint8_t flat_table[256] = {0};
    size_t count = 0;

    for (size_t i = 0; i < key.size; ++i) {
        if (!used[key.data[i]]) {
            used[key.data[i]] = 1;
            flat_table[count++] = key.data[i];
        }
    }
    for (int i = 0; i < 256; ++i) {
        if (!used[i]) flat_table[count++] = static_cast<uint8_t>(i);
    }
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            table[r][c] = flat_table[r * 16 + c];
        }
    }
    secure_clear_memory(used, sizeof(used));
    secure_clear_memory(flat_table, sizeof(flat_table));
}

static void find_position(uint8_t table[16][16], uint8_t val, int& row, int& col) {
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            if (table[r][c] == val) { row = r; col = c; return; }
        }
    }
}

extern "C" {

const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = { "playfair", 16 };
    return &info;
}

size_t get_output_size(size_t input_size, int operation_type) {
    if (operation_type == 1) { 
        size_t pad_len = 2 - (input_size % 2);
        if (pad_len == 2) pad_len = 0; 
        return input_size + (pad_len == 0 ? 2 : pad_len);
    }
    return input_size;
}

int validate_key(ConstBuffer key) {
    if (key.size < 16) return 1;
    return 0;
}

int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (!output || !output->data) return 1;
    if (validate_key(key) != 0) return 3;

    size_t pad_len = 2 - (input.size % 2);
    if (pad_len == 2) pad_len = 0;
    size_t padded_size = input.size + (pad_len == 0 ? 2 : pad_len);
    
    if (output->size < padded_size) return 1;

    uint8_t table[16][16];
    init_table(key, table);

    vector<uint8_t> padded_input(padded_size, static_cast<uint8_t>(pad_len == 0 ? 2 : pad_len));
    memcpy(padded_input.data(), input.data, input.size);

    for (size_t i = 0; i < padded_size; i += 2) {
        int r1, c1, r2, c2;
        find_position(table, padded_input[i], r1, c1);
        find_position(table, padded_input[i+1], r2, c2);

        if (r1 == r2) {
            output->data[i]   = table[r1][(c1 + 1) % 16];
            output->data[i+1] = table[r2][(c2 + 1) % 16];
        } else if (c1 == c2) {
            output->data[i]   = table[(r1 + 1) % 16][c1];
            output->data[i+1] = table[(r2 + 1) % 16][c2];
        } else {
            output->data[i]   = table[r1][c2];
            output->data[i+1] = table[r2][c1];
        }
    }

    secure_clear_memory(table, sizeof(table));
    output->size = padded_size;
    return 0;
}

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (!output || !output->data || input.size % 2 != 0 || output->size < input.size) return 1;
    if (validate_key(key) != 0) return 3;

    uint8_t table[16][16];
    init_table(key, table);

    for (size_t i = 0; i < input.size; i += 2) {
        int r1, c1, r2, c2;
        find_position(table, input.data[i], r1, c1);
        find_position(table, input.data[i+1], r2, c2);

        if (r1 == r2) {
            output->data[i]   = table[r1][(c1 + 15) % 16];
            output->data[i+1] = table[r2][(c2 + 15) % 16];
        } else if (c1 == c2) {
            output->data[i]   = table[(r1 + 15) % 16][c1];
            output->data[i+1] = table[(r2 + 15) % 16][c2];
        } else {
            output->data[i]   = table[r1][c2];
            output->data[i+1] = table[r2][c1];
        }
    }

    uint8_t pad_val = output->data[input.size - 1];
    if (pad_val > 2 || pad_val == 0) {
        output->size = input.size; 
    } else {
        bool valid_pad = true;
        for (uint8_t i = 1; i <= pad_val; ++i) {
            if (output->data[input.size - i] != pad_val) {
                valid_pad = false;
                break;
            }
        }
        output->size = valid_pad ? (input.size - pad_val) : input.size;
    }

    secure_clear_memory(table, sizeof(table));
    return 0;
}

}