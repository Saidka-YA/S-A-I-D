#pragma once
#include <string>

// Шифрование Бифида 
std::string encryptBifid(const std::string& plaintext);

// Расшифровка Бифида
std::string decryptBifid(const std::string& ciphertext);