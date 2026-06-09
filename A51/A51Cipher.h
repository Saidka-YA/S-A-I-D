#ifndef A51CIPHER_H
#define A51CIPHER_H
#pragma once
#include <vector>
#include <cstdint>

uint8_t getBit(uint32_t reg, int pos);
void clockR1(uint32_t& r1);
void clockR2(uint32_t& r2);
void clockR3(uint32_t& r3);
void clockMajority(uint32_t& r1, uint32_t& r2, uint32_t& r3);
void initA51(uint32_t& r1, uint32_t& r2, uint32_t& r3, uint8_t key[8], 
             uint32_t frameNumber);
uint8_t generateBit(uint32_t& r1, uint32_t& r2, uint32_t& r3);
void generateKeystream(uint32_t& r1, uint32_t& r2, uint32_t& r3, 
                       std::vector<uint8_t>& keystream, size_t length);

#endif