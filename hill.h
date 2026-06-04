#ifndef HILL_H
#define HILL_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>

using namespace std;

using Matrix = vector<vector<int>>;

// Генерация алфавита для шифра
vector<char32_t> genalphabet();

// Вспомогательная математика
int mod(int a, int m);
int gcd_euclidext(int a, int b, int& u);
int modInverse(int a, int m);

// Перевод символов в индексы и обратно
int charToIndex(char32_t c, const vector<char32_t>& alphabet);
char32_t indexToChar(int i, const vector<char32_t>& alphabet);

// Матричные операции
int matDet(const Matrix& A, int m);
Matrix invMatrix(const Matrix& A, int m);
bool isRightKey(const Matrix& A, int m);
Matrix keyFromWord(const string& word, int n, const vector<char32_t>& alphabet);

vector<char32_t> to_codes(const string& str);
string to_text(const vector<char32_t>& codes);

// Файловые операции с ключом
void saveKey(const Matrix& K, const string& filename);
Matrix loadKey(const string& filename);

// Шифрование / дешифрование
string encrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet);
string decrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet, size_t len);

#endif
