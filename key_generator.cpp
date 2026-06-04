#include "key_generator.h"
#include <random>

std::vector<uint8_t> KeyGenerator::generate_key(size_t length) {
    std::vector<uint8_t> key(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 255);
    
    for (size_t i = 0; i < length; ++i) {
        key[i] = static_cast<uint8_t>(dist(gen));
    }
    return key;
}