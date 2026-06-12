#include "crypto_interface.h"
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// НОД для проверки обратимости a
static int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Расширенный алгоритм Евклида для поиска a^(-1) mod m
static int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;
}

extern "C" {

const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = { "affine", 2 }; // 2 байта: a и b
    return &info;
}

size_t get_output_size(size_t input_size, int /*operation_type*/) {
    // Аффинный шифр сохраняет размер
    return input_size;
}

int validate_key(ConstBuffer key) {
    if (key.size < 2) return 1; // Нужно минимум 2 байта
    
    int a = key.data[0];
    int m = 26;
    
    // Проверка: a должно быть взаимно просто с 26
    if (gcd(a, m) != 1) {
        return 2; // Неверный ключ
    }
    
    return 0; // Ключ валиден
}

int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (!output || !output->data) return 1;
    if (validate_key(key) != 0) return 3;
    if (output->size < input.size) return 1;

    int a = key.data[0];
    int b = key.data[1];
    int m = 26;

    for (size_t i = 0; i < input.size; ++i) {
        uint8_t c = input.data[i];
        
        // Обрабатываем только буквы
        if (c >= 'a' && c <= 'z') {
            int x = c - 'a';
            int encrypted = (a * x + b) % m;
            output->data[i] = static_cast<uint8_t>(encrypted + 'a');
        } else if (c >= 'A' && c <= 'Z') {
            int x = c - 'A';
            int encrypted = (a * x + b) % m;
            output->data[i] = static_cast<uint8_t>(encrypted + 'A');
        } else {
            // Не буквы оставляем как есть
            output->data[i] = c;
        }
    }

    output->size = input.size;
    return 0;
}

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (!output || !output->data) return 1;
    if (output->size < input.size) return 1;
    
    int a = key.data[0];
    int b = key.data[1];
    int m = 26;
    
    // ПРОВЕРКА 1: a должно быть нечётным
    if (a % 2 == 0) {
        return 2; // Неверный ключ
    }
    
    // ПРОВЕРКА 2: a не должно делиться на 13
    if (a % 13 == 0) {
        return 2;
    }
    
    // ПРОВЕРКА 3: общая проверка через НОД
    if (gcd(a, m) != 1) {
        return 2;
    }
    
    // Находим обратный элемент a^(-1) mod m
    int a_inv = modInverse(a, m);
    if (a_inv == -1) {
        return 3; // Внутренняя ошибка
    }

    for (size_t i = 0; i < input.size; ++i) {
        uint8_t c = input.data[i];
        
        if (c >= 'a' && c <= 'z') {
            int y = c - 'a';
            int decrypted = (a_inv * (y - b + m)) % m;
            output->data[i] = static_cast<uint8_t>(decrypted + 'a');
        } else if (c >= 'A' && c <= 'Z') {
            int y = c - 'A';
            int decrypted = (a_inv * (y - b + m)) % m;
            output->data[i] = static_cast<uint8_t>(decrypted + 'A');
        } else {
            output->data[i] = c;
        }
    }

    output->size = input.size;
    return 0;
}

} // extern "C"