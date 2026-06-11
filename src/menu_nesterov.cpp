#include "menu.h"
#include "hill.h"
#include "vernam.h"
#include "filef.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Шифрование Хилла
static void run_hill(bool encrypt, const vector<char32_t>& alphabet) {
    if (encrypt) {
        int n = input_int("Размер ключевой матрицы (2 или 3): ", 2, 8);
        cout << "Введите ключевое слово: ";
        string keyword;
        getline(cin, keyword);

        Matrix K = keyFromWord(keyword, n, alphabet);
        if (K.empty()) return;

        cout << "Путь для сохранения ключа (Enter — пропустить): ";
        string keyfile;
        getline(cin, keyfile);
        if (!keyfile.empty()) {
            saveKey(K, keyfile);
            cout << "Ключ сохранён в '" << keyfile << "'\n";
        }

        cout << "Введите текст (или путь к файлу): ";
        string src;
        getline(cin, src);
        string text;
        // Если файл существует — читаем
        if (!readFile(src, text)) text = src;

        size_t len = to_codes(text).size();
        string encrypted = hillEncrypt(text, K, alphabet);
        cout << "Шифротекст:\n" << encrypted << "\n";

        cout << "Путь для сохранения шифротекста (Enter — пропустить): ";
        string encfile;
        getline(cin, encfile);
        if (!encfile.empty())
            writeFile(encfile, to_string(len) + "\n" + encrypted);

    } else {
        string keyfile = input_path("Путь к файлу ключа");
        Matrix K = loadKey(keyfile);
        if (K.empty()) return;

        string encfile = input_path("Путь к файлу шифротекста");
        string encContent;
        if (!readFile(encfile, encContent)) return;

        size_t newline = encContent.find('\n');
        if (newline == string::npos) {
            cerr << "Ошибка: неверный формат файла шифротекста!\n";
            return;
        }
        size_t len = stoul(encContent.substr(0, newline));
        string encrypted = encContent.substr(newline + 1);
        string decrypted = hillDecrypt(encrypted, K, alphabet, len);
        cout << "Расшифрованный текст:\n" << decrypted << "\n";

        cout << "Путь для сохранения результата (Enter — пропустить): ";
        string decfile;
        getline(cin, decfile);
        if (!decfile.empty()) writeFile(decfile, decrypted);
    }
}

// Шифрование Вернама
static void run_vernam(bool encrypt, const vector<char32_t>& alphabet) {
    if (encrypt) {
        cout << "Введите текст (или путь к файлу): ";
        string src;
        getline(cin, src);
        string text;
        if (!readFile(src, text)) text = src;

        vector<int> key = vernamKeyFromText(text, alphabet);

        cout << "Путь для сохранения ключа (Enter — пропустить): ";
        string keyfile;
        getline(cin, keyfile);
        if (!keyfile.empty()) {
            saveVernamKey(key, keyfile);
            cout << "Ключ сохранён в '" << keyfile << "'\n";
        }

        string encrypted = vernamEncrypt(text, key, alphabet);
        cout << "Шифротекст:\n" << encrypted << "\n";

        cout << "Путь для сохранения шифротекста (Enter — пропустить): ";
        string encfile;
        getline(cin, encfile);
        if (!encfile.empty()) writeFile(encfile, encrypted);

    } else {
        string keyfile = input_path("Путь к файлу ключа");
        vector<int> key = loadVernamKey(keyfile);
        if (key.empty()) return;

        string encfile = input_path("Путь к файлу шифротекста");
        string encContent;
        if (!readFile(encfile, encContent)) return;

        string decrypted = vernamDecrypt(encContent, key, alphabet);
        cout << "Расшифрованный текст:\n" << decrypted << "\n";

        cout << "Путь для сохранения результата (Enter — пропустить): ";
        string decfile;
        getline(cin, decfile);
        if (!decfile.empty()) writeFile(decfile, decrypted);
    }
}

void menu_nesterov() {
    cout << "\n=== Хилл / Вернам ===\n";
    cout << "  1. Шифр Хилла — зашифровать\n";
    cout << "  2. Шифр Хилла — расшифровать\n";
    cout << "  3. Шифр Вернама — зашифровать\n";
    cout << "  4. Шифр Вернама — расшифровать\n";
    cout << "  0. Назад\n";

    int choice = input_int("Выбор: ", 0, 4);
    if (choice == 0) return;

    vector<char32_t> alphabet = genalphabet();

    switch (choice) {
        case 1: run_hill(true,  alphabet); break;
        case 2: run_hill(false, alphabet); break;
        case 3: run_vernam(true,  alphabet); break;
        case 4: run_vernam(false, alphabet); break;
    }
}
