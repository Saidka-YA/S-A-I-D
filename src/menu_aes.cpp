#include "menu.h"
#include "AES128.h"
#include "CFBmode.h"
#include "utilities.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace aes;

void menu_aes() {
    cout << "\n=== AES-128 (CFB) ===\n";
    cout << "  1. Зашифровать файл\n";
    cout << "  2. Расшифровать файл\n";
    cout << "  3. Зашифровать текст\n";
    cout << "  0. Назад\n";

    int choice = input_int("Выбор: ", 0, 3);
    if (choice == 0) return;

    string keyPath = "aesKey.bin";
    uint8_t key[16];
    uint8_t expandedKey[176];

    if (choice == 1) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        vector<uint8_t> data;
        if (!readFile(inputPath, data)) { cerr << "Ошибка чтения файла\n"; return; }

        generateKey(key);
        if (!saveKey(keyPath, key)) { cerr << "Ошибка сохранения ключа\n"; return; }
        expandRoundKey(key, expandedKey);
        Padding(data);

        vector<uint8_t> ciphertext;
        encryptCFB(data, ciphertext, expandedKey);

        if (!writeFile(outputPath, ciphertext)) { cerr << "Ошибка записи файла\n"; return; }
        cout << "Файл зашифрован. Ключ сохранён в " << keyPath << "\n";

    } else if (choice == 2) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        if (!loadKey(keyPath, key)) { cerr << "Ошибка загрузки ключа\n"; return; }
        expandRoundKey(key, expandedKey);

        vector<uint8_t> ciphertext;
        if (!readFile(inputPath, ciphertext)) { cerr << "Ошибка чтения файла\n"; return; }

        vector<uint8_t> plaintext;
        decryptCFB(ciphertext, plaintext, expandedKey);
        deletePadding(plaintext);

        if (!writeFile(outputPath, plaintext)) { cerr << "Ошибка записи файла\n"; return; }
        cout << "Файл расшифрован.\n";

    } else {
        cout << "Введите текст для шифрования: ";
        string text;
        getline(cin, text);

        vector<uint8_t> data(text.begin(), text.end());
        generateKey(key);
        saveKey(keyPath, key);
        expandRoundKey(key, expandedKey);
        Padding(data);

        vector<uint8_t> ciphertext;
        encryptCFB(data, ciphertext, expandedKey);
        cout << "\nЗашифрованный текст (hex):\n" << hexDisplay(ciphertext) << "\n";

        vector<uint8_t> plaintext;
        decryptCFB(ciphertext, plaintext, expandedKey);
        deletePadding(plaintext);
        cout << "\nРасшифрованный текст:\n"
             << string(plaintext.begin(), plaintext.end()) << "\n";
    }
}
