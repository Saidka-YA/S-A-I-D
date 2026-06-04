#include "hill.h"
#include <random>

// Генерация алфавита для шифра
vector<char32_t> genalphabet() {
    vector<char32_t> alphabet;

    // A-Z
    for (char32_t c = 0x41; c <= 0x5A; ++c) alphabet.push_back(c);
    // a-z
    for (char32_t c = 0x61; c <= 0x7A; ++c) alphabet.push_back(c);
    // А-Я + Ё
    for (char32_t c = 0x410; c <= 0x42F; ++c) alphabet.push_back(c);
    alphabet.push_back(0x401); // Ё
    // а-я + ё
    for (char32_t c = 0x430; c <= 0x44F; ++c) alphabet.push_back(c);
    alphabet.push_back(0x451); // ё
    // 0-9
    for (char32_t c = 0x30; c <= 0x39; ++c) alphabet.push_back(c);

    // Знаки препинания
    const char32_t punct[] = {
        0x20, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A,
        0x28, 0x29, 0x2D, 0x5F, 0x3D, 0x2B, 0x5C, 0x7C, 0x2F,
        0x22, 0x3A, 0x3B, 0x2E, 0x2C, 0x3F, 0xA, 0xD, 0x9,
        0xAB, 0xBB, 0x2014, 0x2013, 0x2026, 0x2018, 0x2019, 0x201C, 0x201D
    };
    for (char32_t c : punct) alphabet.push_back(c);

    return alphabet;
}

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

// UTF-8 строка → вектор кодпоинтов
vector<char32_t> to_codes(const string& str) {
    vector<char32_t> codes;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c <= 0x7F) {
            codes.push_back((char32_t)c);
            i += 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x1F) << 6) | (str[i+1] & 0x3F)));
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F)));
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) | ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F)));
            i += 4;
        } else {
            i += 1;
        }
    }
    return codes;
}

// Вектор кодпоинтов → UTF-8 строка
string to_text(const vector<char32_t>& codes) {
    string result;
    for (char32_t cp : codes) {
        if (cp < 0x80) {
            result += (char)cp;
        } else if (cp < 0x800) {
            result += (char)(0xC0 | (cp >> 6));
            result += (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            result += (char)(0xE0 | (cp >> 12));
            result += (char)(0x80 | ((cp >> 6) & 0x3F));
            result += (char)(0x80 | (cp & 0x3F));
        } else {
            result += (char)(0xF0 | (cp >> 18));
            result += (char)(0x80 | ((cp >> 12) & 0x3F));
            result += (char)(0x80 | ((cp >> 6) & 0x3F));
            result += (char)(0x80 | (cp & 0x3F));
        }
    }
    return result;
}

// Генерация ключевой матрицы из ключевого слова
Matrix keyFromWord(const string& word, int n, const vector<char32_t>& alphabet) {
    int m = alphabet.size();

    // Вычисляем seed из ключевого слова
    vector<char32_t> codes = to_codes(word);
    size_t seed = 0;
    for (char32_t c : codes)
        seed ^= hash<uint32_t>{}((uint32_t)c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    mt19937 rng(seed); // генератор на основе seed
    uniform_int_distribution<int> dist(0, m - 1);

    // Генерируем матрицы пока не найдём подходящую
    Matrix K(n, vector<int>(n));
    do {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                K[i][j] = dist(rng);
    } while (!isRightKey(K, m));

    return K;
}

// Сохранение матрицы в файл
void saveKey(const Matrix& K, const string& filename) {
    ofstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return; }
    f << K.size() << "\n";
    for (auto& row : K) {
        for (int i = 0; i < (int)row.size(); i++) {
            f << row[i];
            if (i + 1 < (int)row.size()) f << " ";
        }
        f << "\n";
    }
}

// Загрузка матрицы из файла
Matrix loadKey(const string& filename) {
    ifstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return {}; }
    int n;
    f >> n;
    Matrix K(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            f >> K[i][j];
    return K;
}

// Шифрование
string encrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet) {
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
string decrypt(const string& text, const Matrix& K, const vector<char32_t>& alphabet, size_t len) {
    if (!isRightKey(K, alphabet.size())) {
        cerr << "Ошибка: ключ не подходит для дешифрования!\n";
        return "";
    }
    Matrix Kinv = invMatrix(K, alphabet.size());
    string decrypted = encrypt(text, Kinv, alphabet);

    vector<char32_t> codes = to_codes(decrypted);
    if (len < codes.size()) codes.resize(len);
    
    return to_text(codes);
}
