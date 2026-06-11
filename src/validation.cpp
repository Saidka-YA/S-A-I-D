/*
 * validation.cpp — реализация общих функций валидации ввода.
 * Компилируется с -fPIC и линкуется в каждую .so статически.
 */
#include "validation.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <libgen.h>
#include <cstring>

using namespace std;

// ── Внутренняя утилита: очистить буфер cin ────────────────────────────────
static void flush_cin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ── Внутренняя утилита: проверить что директория пути существует ──────────
static bool parent_dir_exists(const string& path) {
    if (path.empty()) return false;

    char buf[4096];
    strncpy(buf, path.c_str(), sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    const char* dir = dirname(buf);

    struct stat st;
    return (stat(dir, &st) == 0 && S_ISDIR(st.st_mode));
}

// ─────────────────────────────────────────────────────────────────────────
int val_input_int(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin.peek() == '\n' || cin.eof()) {
            flush_cin();
            cerr << "  [!] Ошибка: пустой ввод. Введите целое число от "
                 << lo << " до " << hi << ".\n";
            continue;
        }
        if (cin >> v) {
            flush_cin();
            if (v >= lo && v <= hi) return v;
            cerr << "  [!] Ошибка: число " << v
                 << " вне допустимого диапазона ["
                 << lo << ", " << hi << "]. Попробуйте снова.\n";
        } else {
            flush_cin();
            cerr << "  [!] Ошибка: введены не числовые символы. "
                    "Введите целое число от " << lo << " до " << hi << ".\n";
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────
string val_input_nonempty(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) {
            // EOF — дать шанс
            cin.clear();
            cerr << "  [!] Ошибка чтения ввода. Попробуйте снова.\n";
            continue;
        }
        // Убрать ведущие/хвостовые пробелы
        size_t l = s.find_first_not_of(" \t\r");
        size_t r = s.find_last_not_of(" \t\r");
        if (l == string::npos) {
            cerr << "  [!] Ошибка: текст не может быть пустым. "
                    "Введите что-нибудь.\n";
            continue;
        }
        s = s.substr(l, r - l + 1);
        return s;
    }
}

// ─────────────────────────────────────────────────────────────────────────
string val_input_existing_file(const string& prompt) {
    string path;
    while (true) {
        cout << prompt << ": ";
        if (!getline(cin, path)) { cin.clear(); continue; }

        if (path.empty()) {
            cerr << "  [!] Ошибка: путь не может быть пустым.\n";
            continue;
        }

        ifstream f(path, ios::binary);
        if (!f) {
            cerr << "  [!] Ошибка: файл '" << path
                 << "' не найден или нет прав на чтение. "
                    "Проверьте путь и попробуйте снова.\n";
            continue;
        }

        // Проверим что файл не пустой
        f.seekg(0, ios::end);
        if (f.tellg() == 0) {
            cerr << "  [!] Ошибка: файл '" << path << "' пустой.\n";
            continue;
        }

        return path;
    }
}

// ─────────────────────────────────────────────────────────────────────────
string val_input_writable_file(const string& prompt) {
    string path;
    while (true) {
        cout << prompt << ": ";
        if (!getline(cin, path)) { cin.clear(); continue; }

        if (path.empty()) {
            cerr << "  [!] Ошибка: путь не может быть пустым.\n";
            continue;
        }

        if (!parent_dir_exists(path)) {
            cerr << "  [!] Ошибка: директория для файла '"
                 << path << "' не существует.\n";
            continue;
        }

        ofstream f(path, ios::binary | ios::app);
        if (!f) {
            cerr << "  [!] Ошибка: невозможно создать/открыть файл '"
                 << path << "'. Проверьте права доступа.\n";
            continue;
        }

        return path;
    }
}

// ─────────────────────────────────────────────────────────────────────────
string val_input_optional_file(const string& prompt) {
    string path;
    while (true) {
        cout << prompt << " (Enter — пропустить): ";
        if (!getline(cin, path)) { cin.clear(); return ""; }
        if (path.empty()) return "";

        if (!parent_dir_exists(path)) {
            cerr << "  [!] Ошибка: директория для файла '"
                 << path << "' не существует.\n";
            continue;
        }

        ofstream f(path, ios::binary | ios::app);
        if (!f) {
            cerr << "  [!] Ошибка: невозможно создать файл '"
                 << path << "'. Проверьте права доступа.\n";
            continue;
        }

        return path;
    }
}

// ─────────────────────────────────────────────────────────────────────────
string val_input_keyword(const string& prompt) {
    string s;
    while (true) {
        cout << prompt << ": ";
        if (!getline(cin, s)) { cin.clear(); continue; }

        if (s.empty()) {
            cerr << "  [!] Ошибка: ключевое слово не может быть пустым.\n";
            continue;
        }

        bool ok = true;
        for (unsigned char c : s) {
            if (!isprint(c) && c < 0x80) { ok = false; break; }
        }
        if (!ok) {
            cerr << "  [!] Ошибка: ключевое слово содержит "
                    "непечатаемые символы. Введите снова.\n";
            continue;
        }

        return s;
    }
}

// ─────────────────────────────────────────────────────────────────────────
vector<uint8_t> val_input_hex_key(const string& prompt, size_t key_len) {
    string s;
    while (true) {
        cout << prompt << " (hex, " << key_len * 2 << " символов): ";
        if (!getline(cin, s)) { cin.clear(); continue; }

        // Убрать пробелы
        string clean;
        for (char c : s) if (c != ' ' && c != '\t') clean += c;

        if (clean.size() != key_len * 2) {
            cerr << "  [!] Ошибка: ожидается ровно " << key_len * 2
                 << " hex-символов (введено "
                 << clean.size() << ").\n";
            continue;
        }

        bool ok = true;
        for (char c : clean) {
            if (!isxdigit((unsigned char)c)) { ok = false; break; }
        }
        if (!ok) {
            cerr << "  [!] Ошибка: недопустимые символы. "
                    "Используйте только 0-9, a-f, A-F.\n";
            continue;
        }

        vector<uint8_t> key(key_len);
        for (size_t i = 0; i < key_len; ++i) {
            unsigned int byte_val;
            istringstream ss(clean.substr(i * 2, 2));
            ss >> hex >> byte_val;
            key[i] = static_cast<uint8_t>(byte_val);
        }
        return key;
    }
}

// ─────────────────────────────────────────────────────────────────────────
vector<uint8_t> val_input_affine_key() {
    vector<uint8_t> key(2);

    // Ввод a: нечётное число от 1 до 255
    while (true) {
        int a = val_input_int(
            "  Параметр 'a' (нечётное число 1–255): ", 1, 255);
        if (a % 2 == 0) {
            cerr << "  [!] Ошибка: параметр 'a' должен быть нечётным "
                    "(иначе шифр необратим). Введите нечётное число.\n";
            continue;
        }
        key[0] = static_cast<uint8_t>(a);
        break;
    }

    // Ввод b: любое число 0–255
    int b = val_input_int("  Параметр 'b' (сдвиг 0–255): ", 0, 255);
    key[1] = static_cast<uint8_t>(b);

    return key;
}
