#pragma once
#include <string>

// Перевод байта в Hex-строку
std::string byteToHex(unsigned char byte);

// Перевод Hex-строки в байт
unsigned char hexToByte(const std::string& hex);