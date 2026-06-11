#include "menu.h"
#include "RSAblock.h"
#include "RSAutilities.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace rsa;

void menu_rsa() {
    cout << "\n=== RSA ===\n";
    cout << "  1. Зашифровать файл\n";
    cout << "  2. Расшифровать файл\n";
    cout << "  3. Зашифровать текст\n";
    cout << "  0. Назад\n";

    int choice = input_int("Выбор: ", 0, 3);
    if (choice == 0) return;

    string keyPath = "rsaKey.bin";

    mpz_t n, e, d;
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    if (choice == 1) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        vector<uint8_t> data;
        if (!readFile(inputPath, data)) {
            cerr << "Ошибка чтения файла\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }

        cout << "Генерация ключей...\n";
        generateKeys(n, e, d);
        if (!saveKey(keyPath, n, e, d)) {
            cerr << "Ошибка сохранения ключей\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }

        Padding(data);
        cout << "Шифрование...\n";
        vector<uint8_t> ciphertext;
        encryptData(data, ciphertext, n, e);

        if (!writeFile(outputPath, ciphertext)) {
            cerr << "Ошибка записи файла\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }
        cout << "Файл зашифрован. Ключ сохранён в " << keyPath << "\n";

    } else if (choice == 2) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        if (!loadKey(keyPath, n, e, d)) {
            cerr << "Ошибка загрузки ключей\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }

        vector<uint8_t> ciphertext;
        if (!readFile(inputPath, ciphertext)) {
            cerr << "Ошибка чтения файла\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }

        cout << "Расшифрование...\n";
        vector<uint8_t> plaintext;
        decryptData(ciphertext, plaintext, n, d);
        deletePadding(plaintext);

        if (!writeFile(outputPath, plaintext)) {
            cerr << "Ошибка записи файла\n";
            mpz_clear(n); mpz_clear(e); mpz_clear(d);
            return;
        }
        cout << "Файл расшифрован.\n";

    } else {
        cout << "Введите текст для шифрования: ";
        string text;
        getline(cin, text);

        vector<uint8_t> data(text.begin(), text.end());

        cout << "Генерация ключей...\n";
        generateKeys(n, e, d);
        saveKey(keyPath, n, e, d);
        Padding(data);

        cout << "Шифрование...\n";
        vector<uint8_t> ciphertext;
        encryptData(data, ciphertext, n, e);
        cout << "\nЗашифрованный текст (hex):\n" << hexDisplay(ciphertext) << "\n";

        cout << "\nРасшифрование...\n";
        vector<uint8_t> plaintext;
        decryptData(ciphertext, plaintext, n, d);
        deletePadding(plaintext);
        cout << "\nРасшифрованный текст:\n"
             << string(plaintext.begin(), plaintext.end()) << "\n";
    }

    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);
}
