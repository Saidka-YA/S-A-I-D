#pragma once
#include <string>

// Полный цикл шифрования ADFGX
std::string encryptADFGX(const std::string& plaintext, const std::string& keyword);

// Полный цикл расшифрования ADFGX
std::string decryptADFGX(const std::string& ciphertext, const std::string& keyword);