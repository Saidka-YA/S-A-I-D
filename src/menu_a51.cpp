#include "menu.h"
#include "A51EncryptDecrypt.h"
#include "A51utilities.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace a51;

void menu_a51() {
    cout << "\n=== A5/1 ===\n";
    cout << "  1. Зашифровать файл\n";
    cout << "  2. Расшифровать файл\n";
    cout << "  3. Зашифровать текст\n";
    cout << "  0. Назад\n";

    int choice = input_int("Выбор: ", 0, 3);
    if (choice == 0) return;

    string keyPath = "a51Key.bin";
    uint8_t key[8];

    if (choice == 1) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        vector<uint8_t> data;
        if (!readFile(inputPath, data)) { cerr << "Ошибка чтения файла\n"; return; }

        generateKey(key);
        if (!saveKey(keyPath, key)) { cerr << "Ошибка сохранения ключа\n"; return; }

        Padding(data);
        vector<uint8_t> ciphertext;
        encryptA51(data, ciphertext, key);

        if (!writeFile(outputPath, ciphertext)) { cerr << "Ошибка записи файла\n"; return; }
        cout << "Файл зашифрован. Ключ сохранён в " << keyPath << "\n";

    } else if (choice == 2) {
        string inputPath  = input_path("Путь к файлу");
        string outputPath = input_path("Путь для сохранения результата");

        if (!loadKey(keyPath, key)) { cerr << "Ошибка загрузки ключа\n"; return; }

        vector<uint8_t> ciphertext;
        if (!readFile(inputPath, ciphertext)) { cerr << "Ошибка чтения файла\n"; return; }

        vector<uint8_t> plaintext;
        decryptA51(ciphertext, plaintext, key);
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
        Padding(data);

        vector<uint8_t> ciphertext;
        encryptA51(data, ciphertext, key);
        cout << "\nЗашифрованный текст (hex):\n" << hexDisplay(ciphertext) << "\n";

        vector<uint8_t> plaintext;
        decryptA51(ciphertext, plaintext, key);
        deletePadding(plaintext);
        cout << "\nРасшифрованный текст:\n"
             << string(plaintext.begin(), plaintext.end()) << "\n";
    }
}
