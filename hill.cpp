#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

// Генерация алфавита для шифра
vector<char32_t> genalphabet() {
    vector<char32_t> alphabet;
    for (char32_t c = U'A'; c <= U'Z'; ++c) alphabet.push_back(c);
    for (char32_t c = U'a'; c <= U'z'; ++c) alphabet.push_back(c);
    for (char32_t c = U'А'; c <= U'Я'; ++c) alphabet.push_back(c);
    for (char32_t c = U'а'; c <= U'я'; ++c) alphabet.push_back(c);
    for (char32_t c = U'0'; c <= U'9'; ++c) alphabet.push_back(c);
    const char32_t punct[] = {
        U' ', U'!', U'@', U'#', U'$', U'%', U'^', U'&', U'*',
        U'(', U')', U'-', U'_', U'=', U'+', U'\\', U'|', U'/',
        U'"', U':', U';', U'.', U',', U'?'
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
    int gcd = gcd_euclidext(mod(a, m), m, u); // mod на случай отрицательного a
    if (gcd != 1) {
        cerr << "Обратный элемент не существует!\n";
        return 0;
    }
    return mod(u, m);
}

int main() {
    setlocale(LC_ALL, "Rus");
    
    return 0;
}