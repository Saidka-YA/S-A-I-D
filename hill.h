#ifndef HILL_H
#define HILL_H
#include <vector>
#include <string>
#include "filef.h"
using namespace std;

using Matrix = vector<vector<int>>;

// вспомогательная математика (всегда по модулю 256)
int mod(int a, int m);
int gcd_euclidext(int a, int b, int& u);
int modInverse(int a, int m);
int matDet(const Matrix& A, int m);
Matrix invMatrix(const Matrix& A, int m);
bool isRightKey(const Matrix& A, int m);

// генерация ключа и шифрование/дешифрование (без алфавита)
Matrix keyFromWord(const string& word, int n);
string hillEncrypt(const string& text, const Matrix& K);
string hillDecrypt(const string& text, const Matrix& K, size_t len);

#endif