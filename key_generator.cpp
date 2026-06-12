#include "key_generator.h"
#include <random>
#include <stdexcept>

using namespace std;

vector<uint8_t> KeyGenerator::generate_secure_key(size_t length) {
    vector<uint8_t> key(length);
    if (length == 0) return key;

    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint16_t> dist(0, 255);
    
    for (size_t i = 0; i < length; ++i) {
        key[i] = static_cast<uint8_t>(dist(gen));
    }
    return key;
}