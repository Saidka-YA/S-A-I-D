/*
 * libs/lib_hill_vernam.cpp  —  Шифр Хилла + Шифр Вернама
 */
#include "hill.h"
#include "vernam.h"
#include "filef.h"
#include "textproc.h"
#include "plugin_api.h"
#include "validation.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// ── Хилл ──────────────────────────────────────────────────────────────────

static void run_hill(bool do_encrypt) {
    if (do_encrypt) {
        int n = val_input_int("  Размер ключевой матрицы (2–20): ", 2, 20);

        string keyword = val_input_keyword("  Ключевое слово");

        Matrix K = keyFromWord(keyword, n);
        if (K.empty()) {
            cerr << "  [!] Ошибка: не удалось построить ключевую матрицу "
                    "из слова '" << keyword << "'.\n"
                    "      Попробуйте другое ключевое слово.\n";
            return;
        }

        string keyfile = val_input_optional_file("  Файл для сохранения ключа");
        if (!keyfile.empty()) {
            saveKey(K, keyfile);
            cout << "  Ключ сохранён в '" << keyfile << "'\n";
        }

        // Источник текста: файл или консоль
        cout << "  Источник текста (1 — файл, 2 — консоль): ";
        int src = val_input_int("", 1, 2);

        string text;
        if (src == 1) {
            string path = val_input_existing_file("  Путь к файлу");
            if (!readFile(path, text)) {
                cerr << "  [!] Ошибка: не удалось прочитать файл '" << path << "'.\n";
                return;
            }
        } else {
            text = val_input_nonempty("  Введите текст: ");
        }

        size_t len = text.size();
        string encrypted = hillEncrypt(text, K);
        if (src == 2) cout << "  Шифротекст:\n  " << encrypted << "\n";

        string encfile = val_input_optional_file("  Файл для сохранения шифротекста");
        if (!encfile.empty())
            writeFile(encfile, to_string(len) + "\n" + encrypted);

    } else {
        string keyfile = val_input_existing_file("  Файл ключа");
        Matrix K = loadKey(keyfile);
        if (K.empty()) {
            cerr << "  [!] Ошибка: не удалось загрузить ключ из '" << keyfile << "'.\n"
                    "      Убедитесь, что файл создан шифром Хилла и не повреждён.\n";
            return;
        }

        string encfile = val_input_existing_file("  Файл с шифротекстом");
        string encContent;
        if (!readFile(encfile, encContent)) {
            cerr << "  [!] Ошибка: не удалось прочитать файл '" << encfile << "'.\n";
            return;
        }

        size_t nl = encContent.find('\n');
        if (nl == string::npos) {
            cerr << "  [!] Ошибка: неверный формат файла шифротекста.\n"
                    "      Файл должен начинаться со строки с длиной оригинала.\n";
            return;
        }

        size_t len = 0;
        try {
            len = stoul(encContent.substr(0, nl));
        } catch (...) {
            cerr << "  [!] Ошибка: первая строка файла шифротекста "
                    "должна быть числом (длина оригинала).\n";
            return;
        }

        string encrypted = encContent.substr(nl + 1);
        if (encrypted.empty()) {
            cerr << "  [!] Ошибка: шифротекст пустой.\n";
            return;
        }

        string decrypted = hillDecrypt(encrypted, K, len);
        cout << "  Расшифрованный текст:\n  " << decrypted << "\n";

        string decfile = val_input_optional_file("  Файл для сохранения результата");
        if (!decfile.empty()) writeFile(decfile, decrypted);
    }
}

// ── Вернам ────────────────────────────────────────────────────────────────

static void run_vernam(bool do_encrypt) {
    if (do_encrypt) {
        cout << "  Источник текста (1 — файл, 2 — консоль): ";
        int src = val_input_int("", 1, 2);

        string text;
        if (src == 1) {
            string path = val_input_existing_file("  Путь к файлу");
            if (!readFile(path, text)) {
                cerr << "  [!] Ошибка: не удалось прочитать файл '" << path << "'.\n";
                return;
            }
        } else {
            text = val_input_nonempty("  Введите текст: ");
        }

        vector<uint8_t> key = vernamKeyFromText(text);

        string keyfile = val_input_optional_file("  Файл для сохранения ключа");
        if (!keyfile.empty()) {
            saveVernamKey(key, keyfile);
            cout << "  Ключ сохранён в '" << keyfile << "'\n";
        }

        string encrypted = vernamEncrypt(text, key);
        if (src == 2) cout << "  Шифротекст:\n  " << encrypted << "\n";

        string encfile = val_input_optional_file("  Файл для сохранения шифротекста");
        if (!encfile.empty()) writeFile(encfile, encrypted);

    } else {
        string keyfile = val_input_existing_file("  Файл ключа");
        vector<uint8_t> key = loadVernamKey(keyfile);
        if (key.empty()) {
            cerr << "  [!] Ошибка: не удалось загрузить ключ из '" << keyfile << "'.\n"
                    "      Убедитесь, что файл создан шифром Вернама и не повреждён.\n";
            return;
        }

        string encfile = val_input_existing_file("  Файл с шифротекстом");
        string encContent;
        if (!readFile(encfile, encContent)) {
            cerr << "  [!] Ошибка: не удалось прочитать файл '" << encfile << "'.\n";
            return;
        }
        if (encContent.empty()) {
            cerr << "  [!] Ошибка: файл с шифротекстом пустой.\n";
            return;
        }

        string decrypted = vernamDecrypt(encContent, key);
        cout << "  Расшифрованный текст:\n  " << decrypted << "\n";

        string decfile = val_input_optional_file("  Файл для сохранения результата");
        if (!decfile.empty()) writeFile(decfile, decrypted);
    }
}

// ── Точка входа ───────────────────────────────────────────────────────────

extern "C" void plugin_run() {
    cout << "\n=== Хилл / Вернам ===\n";
    cout << "  1. Шифр Хилла — зашифровать\n";
    cout << "  2. Шифр Хилла — расшифровать\n";
    cout << "  3. Шифр Вернама — зашифровать\n";
    cout << "  4. Шифр Вернама — расшифровать\n";
    cout << "  0. Назад\n";

    int choice = val_input_int("Выбор: ", 0, 4);
    if (choice == 0) return;

    switch (choice) {
        case 1: run_hill(true);    break;
        case 2: run_hill(false);   break;
        case 3: run_vernam(true);  break;
        case 4: run_vernam(false); break;
    }
}
