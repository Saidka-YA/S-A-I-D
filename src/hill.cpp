#include "hill.h"
#include <random>
#include <iostream>

// Модуль арифметики шифра Хилла: работаем с байтами, поэтому 256
const int MOD = 256;

// Приведение числа к диапазону [0, m)
int mod(int a, int m) {
    return ((a % m) + m) % m;
}

// Расширенный алгоритм Евклида.
// Возвращает gcd(a, b) и через u — коэффициент Безу: a*u + b*v = gcd(a,b)
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

// Обратный элемент a по модулю m. Возвращает 0, если gcd(a, m) != 1
// (обратного элемента не существует)
int modInverse(int a, int m) {
    int u = 0;
    int gcd = gcd_euclidext(mod(a, m), m, u);
    if (gcd != 1) return 0;
    return mod(u, m);
}

// Определитель квадратной матрицы A по модулю m.
// Используется метод Гаусса: матрица приводится к верхнетреугольному виду,
// определитель — произведение диагональных элементов (с учётом знака от перестановок строк)
int matDet(const Matrix& A, int m) {
    int n = A.size();
    Matrix M = A;
    int det = 1;
    for (int col = 0; col < n; ++col) {
        int nenull = -1;
        for (int row = col; row < n; ++row) {
            // ищем ненулевой, нечётный (обратимый по модулю 256) опорный элемент
            if (mod(M[row][col], m) % 2 != 0) {
                nenull = row;
                break;
            }
        }
        if (nenull == -1) return 0; // подходящего опорного элемента нет — матрица необратима
        if (nenull != col) {
            swap(M[nenull], M[col]);
            det = mod(-det, m); // перестановка строк меняет знак определителя
        }
        det = mod(det * M[col][col], m);
        int nenullInv = modInverse(M[col][col], m);
        // обнуляем элементы под опорным
        for (int row = col + 1; row < n; ++row) {
            int factor = mod(M[row][col] * nenullInv, m);
            for (int k = col; k < n; ++k)
                M[row][k] = mod(M[row][k] - factor * M[col][k], m);
        }
    }
    return det;
}

// Обратная матрица A^-1 по модулю m методом Гаусса-Жордана.
// Строим расширенную матрицу [A | I], приводим левую часть к единичной —
// правая часть становится A^-1. Возвращает пустую матрицу, если A необратима
Matrix invMatrix(const Matrix& A, int m) {
    int n = A.size();
    Matrix M(n, vector<int>(2 * n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) M[i][j] = mod(A[i][j], m);
        M[i][i + n] = 1; // правая половина — единичная матрица
    }
    for (int col = 0; col < n; ++col) {
        int nenull = -1;
        for (int row = col; row < n; ++row) {
            // ищем ненулевой, нечётный (обратимый по модулю 256) опорный элемент
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

        // нормализуем строку опорного элемента (делаем его равным 1)
        int nenullInv = modInverse(M[col][col], m);
        for (int j = 0; j < 2 * n; ++j)
            M[col][j] = mod(M[col][j] * nenullInv, m);

        // обнуляем столбец col во всех остальных строках
        for (int row = 0; row < n; ++row) {
            if (row == col) continue;
            int fac = M[row][col];
            for (int j = 0; j < 2 * n; ++j)
                M[row][j] = mod(M[row][j] - fac * M[col][j], m);
        }
    }
    // правая половина расширенной матрицы — искомая обратная матрица
    Matrix res(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            res[i][j] = M[i][j + n];
    return res;
}

// Матрица годится в качестве ключа Хилла, если её определитель не равен 0
// и обратим по модулю m (т.е. gcd(det, m) == 1) — иначе матрицу нельзя обратить
bool isRightKey(const Matrix& A, int m) {
    int det = matDet(A, m);
    if (det == 0) return false;
    int u = 0;
    return gcd_euclidext(det, m, u) == 1;
}

// Генерация ключевой матрицы n x n из ключевого слова.
// Слово детерминированно превращается в seed для ГПСЧ (mt19937),
// после чего матрица случайно заполняется и проверяется на обратимость;
// при неудаче генерируется новая случайная матрица (повтор до успеха)
Matrix keyFromWord(const string& word, int n) {
    vector<char32_t> codes = to_codes(word);
    size_t seed = 0;
    // комбинируем коды символов слова в единый seed
    for (char32_t c : codes)
        seed ^= hash<uint32_t>{}((uint32_t)c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    mt19937 rng(seed);
    uniform_int_distribution<int> dist(0, MOD - 1);

    Matrix K(n, vector<int>(n));
    do {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                K[i][j] = dist(rng);
    } while (!isRightKey(K, MOD)); // повторяем, пока не получим обратимую матрицу

    return K;
}

// Шифрование: текст дополняется нулевыми байтами до длины, кратной
// размеру матрицы n, затем каждый блок из n байт умножается на матрицу K
// (по модулю 256), результат — новый блок из n байт
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

// Дешифрование: вычисляем обратную матрицу K^-1 и применяем к ней
// тот же алгоритм умножения блоков (hillEncrypt), что и при шифровании.
// После этого убираем нулевой padding, добавленный при шифровании,
// обрезая результат до исходной длины len
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
