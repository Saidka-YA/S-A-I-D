#ifndef A51ENCRYPTDECRYPT_H
#define A51ENCRYPTDECRYPT_H
#pragma once
#include <vector>
#include <cstdint>

void encryptA51(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, 
                 uint8_t key[8]);
void decryptA51(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, 
                 uint8_t key[8]);

#endif