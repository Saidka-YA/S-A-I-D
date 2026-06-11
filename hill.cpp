#include "hill.h"
#include <random>
#include <iostream>

const int MOD = 256;

int mod(int a, int m) {
    return ((a % m) + m) % m;
}

int gcd_euclidext(int a, int b, int& u) {
    if (b == 0) {
        u = 1;
        return a;
    }
    int u_prev = 1, u_curr = 0;
    int r1 = a, r2 = b;
    while (r2 != 0) {
        int q = r1 / r2;
        int temp = r2;
        r2 = r1 - r2 * q;
        r1 = temp;
        temp = u_curr;
        u_curr = u_prev - q * u_curr;
        u_prev = temp;
    }
    u = u_prev;
    return r1;
}

int modInverse(int a, int m) {
    int u = 0;
    int gcd = gcd_euclidext(mod(a, m), m, u);
    if (gcd != 1) return 0;
    return mod(u, m);
}

int matDet(const Matrix& A, int m) {
    int n = A.size();
    Matrix M = A;
    int det = 1;
    for (int col = 0; col < n; ++col) {
        int nenull = -1;
        for (int row = col; row < n; ++row) {
            // ИСПРАВЛЕНО: ищем не просто ненулевой, а нечётный (обратимый по модулю 256)
            if (mod(M[row][col], m) % 2 != 0) {
                nenull = row;
                break;
            }
        }
        if (nenull == -1) return 0;
        if (nenull != col) {
            swap(M[nenull], M[col]);
            det = mod(-det, m);
        }
        det = mod(det * M[col][col], m);
        int nenullInv = modInverse(M[col][col], m);
        for (int row = col + 1; row < n; ++row) {
            int factor = mod(M[row][col] * nenullInv, m);
            for (int k = col; k < n; ++k)
                M[row][k] = mod(M[row][k] - factor * M[col][k], m);
        }
    }
    return det;
}

Matrix invMatrix(const Matrix& A, int m) {
    int n = A.size();
    Matrix M(n, vector<int>(2 * n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) M[i][j] = mod(A[i][j], m);
        M[i][i + n] = 1;
    }
    for (int col = 0; col < n; ++col) {
        int nenull = -1;
        for (int row = col; row < n; ++row) {
            // ИСПРАВЛЕНО: ищем не просто ненулевой, а нечётный (обратимый по модулю 256)
            if (mod(M[row][col], m) % 2 != 0) {
                nenull = row;
                break;
            }
        }
        if (nenull == -1) {
            cerr << "Матрица необратима!\n";
            return {};
        }
        if (nenull != col) swap(M[nenull], M[col]);
        
        int nenullInv = modInverse(M[col][col], m);
        for (int j = 0; j < 2 * n; ++j)
            M[col][j] = mod(M[col][j] * nenullInv, m);
            
        for (int row = 0; row < n; ++row) {
            if (row == col) continue;
            int fac = M[row][col];
            for (int j = 0; j < 2 * n; ++j)
                M[row][j] = mod(M[row][j] - fac * M[col][j], m);
        }
    }
    Matrix res(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            res[i][j] = M[i][j + n];
    return res;
}

bool isRightKey(const Matrix& A, int m) {
    int det = matDet(A, m);
    if (det == 0) return false;
    int u = 0;
    return gcd_euclidext(det, m, u) == 1;
}

Matrix keyFromWord(const string& word, int n) {
    vector<char32_t> codes = to_codes(word);
    size_t seed = 0;
    for (char32_t c : codes)
        seed ^= hash<uint32_t>{}((uint32_t)c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(0, MOD - 1);
    
    Matrix K(n, vector<int>(n));
    do {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                K[i][j] = dist(rng);
    } while (!isRightKey(K, MOD));
    
    return K;
}

string hillEncrypt(const string& text, const Matrix& K) {
    int n = K.size();
    string padded = text;
    
    // дополняем нулевыми байтами (\0) до кратности n
    while (padded.size() % n != 0)
        padded += '\0';
    
    string result = padded;
    for (size_t i = 0; i < padded.size(); i += n) {
        for (int row = 0; row < n; row++) {
            int sum = 0;
            for (int col = 0; col < n; col++) {
                sum += K[row][col] * (uint8_t)padded[i + col];
            }
            result[i + row] = (char)(sum % MOD);
        }
    }
    return result;
}

string hillDecrypt(const string& text, const Matrix& K, size_t len) {
    if (!isRightKey(K, MOD)) {
        cerr << "Ошибка: ключ не подходит для дешифрования!\n";
        return "";
    }
    
    Matrix Kinv = invMatrix(K, MOD);
    string decrypted = hillEncrypt(text, Kinv);
    
    // убираем padding до исходной длины
    if (len < decrypted.size())
        decrypted.resize(len);
        
    return decrypted;
}