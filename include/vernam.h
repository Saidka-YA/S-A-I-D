#ifndef VERNAM_H
#define VERNAM_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "filef.h"

using namespace std;

// генерация случайного ключа по длине текста
vector<uint8_t> vernamKeyFromText(const string& text);

// шифрование и дешифрование - xor каждого байта с ключом
string vernamEncrypt(const string& text, const vector<uint8_t>& key);
string vernamDecrypt(const string& ctext, const vector<uint8_t>& key);

#endif
