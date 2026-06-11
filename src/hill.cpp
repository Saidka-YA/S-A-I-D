#include "hill.h"
#include <random>

// делаем mod отдельно, т.к. могут быть отрицательные числа
int mod(int a, int m) {
    return ((a % m) + m) % m;
}

// Расширенный алгоритм Евклида
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

// Обратный элемент
int modInverse(int a, int m) {
    int u = 0;
    int gcd = gcd_euclidext(mod(a, m), m, u);
    if (gcd != 1) return 0;
    return mod(u, m);
}

// Перевод символов в индексы и обратно
int charToIndex(char32_t c, const vector<char32_t>& alphabet) {
    for (int i = 0; i < (int)alphabet.size(); i++) {
        if (alphabet[i] == c) return i;
    }
    cerr << "Ошибка: символ U+" << hex << uppercase << (uint32_t)c << " не найден в алфавите!\n";
    return -1;
}

char32_t indexToChar(int i, const vector<char32_t>& alphabet) {
    if (i < 0 || i >= (int)alphabet.size()) {
        cerr << "Ошибка: Индекс вне диапазона!\n";
        return U'?';
    }
    return alphabet[i];
}

// Определитель матрицы по модулю (метод Гаусса)
int matDet(const Matrix& A, int m) {
    int n = A.size();
    Matrix M = A;
    int det = 1;

    for (int col = 0; col < n; ++col) {
        int nenull = -1;
        for (int row = col; row < n; ++row) {
            if (mod(M[row][col], m) != 0) {
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

// Обратная матрица по модулю (метод Гаусса-Жордана)
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
            if (mod(M[row][col], m) != 0) {
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

// Проверка ключа
bool isRightKey(const Matrix& A, int m) {
    int det = matDet(A, m);
    if (det == 0) return false;
    int u = 0;
    return gcd_euclidext(det, m, u) == 1;
}

// Генерация ключевой матрицы из ключевого слова
Matrix keyFromWord(const string& word, int n, const vector<char32_t>& alphabet) {
    int m = alphabet.size();

    vector<char32_t> codes = to_codes(word);
    size_t seed = 0;
    for (char32_t c : codes)
        seed ^= hash<uint32_t>{}((uint32_t)c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    mt19937 rng(seed);
    uniform_int_distribution<int> dist(0, m - 1);

    Matrix K(n, vector<int>(n));
    do {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                K[i][j] = dist(rng);
    } while (!isRightKey(K, m));

    return K;
}

// Шифрование
string hillEncrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet) {
    int n = K.size();
    int m = alphabet.size();

    vector<char32_t> codes = to_codes(text);

    while (codes.size() % n != 0)
        codes.push_back(alphabet[0]);

    vector<char32_t> result;

    for (int i = 0; i < (int)codes.size(); i += n) {
        vector<int> block(n);
        for (int j = 0; j < n; j++) {
            block[j] = charToIndex(codes[i + j], alphabet);
            if (block[j] == -1) {
                cerr << "Символ не найден в алфавите!\n";
                return "";
            }
        }

        vector<int> encrypted(n, 0);
        for (int row = 0; row < n; row++)
            for (int col = 0; col < n; col++)
                encrypted[row] = mod(encrypted[row] + K[row][col] * block[col], m);

        for (int j = 0; j < n; j++)
            result.push_back(indexToChar(encrypted[j], alphabet));
    }

    return to_text(result);
}

// Дешифрование
string hillDecrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet, size_t len) {
    if (!isRightKey(K, alphabet.size())) {
        cerr << "Ошибка: ключ не подходит для дешифрования!\n";
        return "";
    }
    Matrix Kinv = invMatrix(K, alphabet.size());
    string decrypted = hillEncrypt(text, Kinv, alphabet);

    vector<char32_t> codes = to_codes(decrypted);
    if (len < codes.size()) codes.resize(len);

    return to_text(codes);
}
