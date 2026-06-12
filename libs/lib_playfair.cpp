/*
 * libs/lib_playfair.cpp
 * Компилируется в libplayfair.so
 * Плейфер-шифр на матрице 16×16 (все 256 байт).
 *
 * Использует crypto_interface.h: get_algorithm_info / encrypt / decrypt
 * Обёрнут в plugin_run() для единого интерфейса плагинов.
 */
#include "plugin_api.h"
#include "validation.h"
#include "crypto_interface.h"
#include "file_processor.h"
#include "key_generator.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

// ── Символы из playfair_cipher.cpp ───────────────────────────────────────
extern "C" {
    const AlgorithmInfo* get_algorithm_info();
    size_t               get_output_size(size_t input_size, int operation_type);
    int                  encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);
    int                  decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);
}

// ── Вспомогательные функции ───────────────────────────────────────────────

static string bytes_to_hex(const vector<uint8_t>& data) {
    ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0 && i % 16 == 0) oss << "\n";
        oss << hex << setw(2) << setfill('0') << (int)data[i] << " ";
    }
    return oss.str();
}

static bool check_crypto_result(int rc, const string& op) {
    if (rc == 0) return true;
    cerr << "  [!] Ошибка " << op << ": ";
    switch (rc) {
        case 1: cerr << "ключ слишком короткий (нужно минимум 16 байт).\n"; break;
        default: cerr << "код ошибки " << rc << ".\n"; break;
    }
    return false;
}

// ── Сохранение / загрузка ключа ───────────────────────────────────────────

static bool save_key(const string& path, const vector<uint8_t>& key) {
    ofstream f(path, ios::binary);
    if (!f) {
        cerr << "  [!] Ошибка: не удалось сохранить ключ в '"
             << path << "'.\n";
        return false;
    }
    f.write(reinterpret_cast<const char*>(key.data()), key.size());
    return true;
}

static vector<uint8_t> load_key(const string& path) {
    try {
        vector<uint8_t> key = FileProcessor::read_file(path);
        if (key.size() < 16) {
            cerr << "  [!] Ошибка: файл ключа '" << path
                 << "' повреждён (меньше 16 байт).\n";
            return {};
        }
        return key;
    } catch (const exception& e) {
        cerr << "  [!] Ошибка загрузки ключа: " << e.what() << "\n";
        return {};
    }
}

// ── Получение ключа от пользователя ──────────────────────────────────────

static vector<uint8_t> acquire_key(bool do_encrypt, const string& default_key_path) {
    if (do_encrypt) {
        cout << "  Способ задания ключа:\n"
                "    1. Ввести ключевое слово (из него строится таблица)\n"
                "    2. Ввести hex-ключ (" << 16 << " байт = 32 hex-символа)\n"
                "    3. Сгенерировать случайный ключ\n";
        int km = val_input_int("  Выбор: ", 1, 3);

        vector<uint8_t> key;
        if (km == 1) {
            string kw = val_input_keyword("  Ключевое слово");
            key.assign(kw.begin(), kw.end());
            // Убедимся что ключ не короче 16 байт — дополним нулями если нужно
            while (key.size() < 16) key.push_back(0);
        } else if (km == 2) {
            key = val_input_hex_key("  Ключ", 16);
        } else {
            key = KeyGenerator::generate_key(16);
            cout << "  Сгенерирован ключ (hex):\n  " << bytes_to_hex(key) << "\n";
        }

        string kp = val_input_optional_file("  Путь для сохранения ключа");
        if (!kp.empty()) {
            if (save_key(kp, key))
                cout << "  Ключ сохранён в '" << kp << "'\n";
        }
        return key;

    } else {
        // Расшифрование: сначала пробуем стандартный файл
        vector<uint8_t> key = load_key(default_key_path);
        if (!key.empty()) return key;

        cerr << "  Стандартный файл ключа '" << default_key_path
             << "' не найден или повреждён.\n"
                "  Введите путь к файлу ключа вручную:\n";
        string kp = val_input_existing_file("  Путь к файлу ключа");
        return load_key(kp);
    }
}

// ── Режим: файл ───────────────────────────────────────────────────────────

static void run_file(bool do_encrypt) {
    const string keyPath = "playfairKey.bin";

    string inputPath  = val_input_existing_file(
        do_encrypt ? "Путь к входному файлу"
                   : "Путь к зашифрованному файлу");

    string outputPath = val_input_writable_file(
        "Путь для сохранения результата");

    vector<uint8_t> key = acquire_key(do_encrypt, keyPath);
    if (key.empty()) return;

    vector<uint8_t> data;
    try {
        data = FileProcessor::read_file(inputPath);
    } catch (const exception& e) {
        cerr << "  [!] Ошибка чтения файла: " << e.what() << "\n";
        return;
    }

    // Playfair обрабатывает пары — если нечётная длина, дополнить нулём
    if (data.size() % 2 != 0) data.push_back(0);

    vector<uint8_t> result(data.size());
    ConstBuffer kb = { key.data(),  key.size()  };
    ConstBuffer ib = { data.data(), data.size() };
    MutBuffer   ob = { result.data(), result.size() };

    int rc = do_encrypt ? encrypt(kb, ib, &ob) : decrypt(kb, ib, &ob);
    if (!check_crypto_result(rc, do_encrypt ? "шифрования" : "расшифрования"))
        return;

    if (!do_encrypt) {
        // Убираем хвостовой нулевой байт, добавленный при выравнивании
        // длины до чётной при шифровании.
        while (!result.empty() && result.back() == 0) {
            result.pop_back();
        }
    }

    try {
        FileProcessor::write_file(outputPath, result);
    } catch (const exception& e) {
        cerr << "  [!] Ошибка записи файла: " << e.what() << "\n";
        return;
    }

    if (do_encrypt) {
        save_key(keyPath, key);
        cout << "  Готово. Ключ автоматически сохранён в " << keyPath << "\n";
    } else {
        cout << "  Файл расшифрован.\n";
    }
}

// ── Режим: текст с консоли ────────────────────────────────────────────────

static void run_text() {
    string text = val_input_nonempty("Введите текст для шифрования: ");

    const string keyPath = "playfairKey.bin";
    vector<uint8_t> key = acquire_key(true, keyPath);
    if (key.empty()) return;

    vector<uint8_t> data(text.begin(), text.end());
    if (data.size() % 2 != 0) data.push_back(0);

    vector<uint8_t> cipher(data.size());
    vector<uint8_t> plain(data.size());

    ConstBuffer kb  = { key.data(),   key.size()    };
    ConstBuffer ib  = { data.data(),  data.size()   };
    MutBuffer   cb  = { cipher.data(), cipher.size() };
    MutBuffer   pb  = { plain.data(),  plain.size()  };

    if (!check_crypto_result(encrypt(kb, ib, &cb), "шифрования")) return;
    cout << "\n  Зашифрованный текст (hex):\n  " << bytes_to_hex(cipher) << "\n";

    ConstBuffer cib = { cipher.data(), cipher.size() };
    if (!check_crypto_result(decrypt(kb, cib, &pb), "расшифрования")) return;
    cout << "\n  Расшифрованный текст:\n  "
         << string(plain.begin(), plain.end()) << "\n";
}

// ── Точка входа плагина ───────────────────────────────────────────────────

extern "C" void plugin_run() {
    const AlgorithmInfo* info = get_algorithm_info();
    cout << "\n=== Шифр Плейфера (" << info->algorithm_name << ", матрица 16×16) ===\n";
    cout << "  1. Зашифровать файл\n";
    cout << "  2. Расшифровать файл\n";
    cout << "  3. Зашифровать текст (консоль)\n";
    cout << "  0. Назад\n";

    int choice = val_input_int("Выбор: ", 0, 3);
    switch (choice) {
        case 1: run_file(true);  break;
        case 2: run_file(false); break;
        case 3: run_text();      break;
        case 0: break;
    }
}
