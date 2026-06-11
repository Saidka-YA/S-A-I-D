#ifndef TEXTPROC_H
#define TEXTPROC_H
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
using namespace std;

// Теперь работаем только с байтами, алфавиты удалены
vector<char32_t> to_codes(const string& str);
string to_text(const vector<char32_t>& codes);

#endif