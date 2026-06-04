#ifndef XOR_H
#define XOR_H

#include <vector>
#include "textproc.h"
#include "filef.h"

using namespace std;

// Ключ из ключевого слова
vector<int> xorKeyFromWord(const string& word, const vector<char32_t>& alphabet);

// операции с ключом
void saveXorKey(const string& key, const string& filename);
string loadXorKey(const string& filename);

// Шифрование дешифрование
string xor_encrypt(const string& text, const string& key, const vector<char32_t>& alphabet);
string xor_decrypt(const string& text, const string& key, const vector<char32_t>& alphabet);

#endif