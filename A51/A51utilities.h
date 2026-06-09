#ifndef A51UTILITIES_H
#define A51UTILITIES_H
#pragma once
#include <vector>
#include <cstdint>
#include <random>
#include <sstream>
#include <fstream>
#include <iomanip>

void Padding(std::vector<uint8_t>& data);
void deletePadding(std::vector<uint8_t>& data);
void generateKey(uint8_t key[8]);
void generateIV(uint8_t iv[4]);
bool saveKey(const std::string& path, uint8_t key[8]);
bool loadKey(const std::string& path, uint8_t key[8]);
bool readFile(const std::string& path, std::vector<uint8_t>& data);
bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
std::string hexDisplay(const std::vector<uint8_t>& data);

#endif