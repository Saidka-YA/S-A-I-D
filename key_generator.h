#ifndef KEY_GENERATOR_H
#define KEY_GENERATOR_H
#include <cstddef> // Подключает size_t
#include <vector>
#include <cstdint>

class KeyGenerator {
public:
    static std::vector<uint8_t> generate_secure_key(size_t length);
};

#endif