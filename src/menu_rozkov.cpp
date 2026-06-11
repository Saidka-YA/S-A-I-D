#include "menu.h"
#include "bifid.h"
#include "adfgx.h"
#include "console_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

// Читает данные: из файла или с консоли
static string get_input_text(bool from_file) {
    if (from_file) {
        string filename = input_path("Имя исходного файла");
        ifstream inFile(filename, ios::binary);
        if (!inFile) throw runtime_error("Не удалось открыть файл: " + filename);
        stringstream buffer;
        buffer << inFile.rdbuf();
        string text = buffer.str();
        if (text.empty()) throw invalid_argument("Исходный файл пуст");
        return text;
    } else {
        cout << "Введите текст: ";
        string text;
        getline(cin, text);
        if (text.empty()) throw invalid_argument("Текст не может быть пустым");
        return text;
    }
}

// Сохраняет результат в файл
static void save_result(const string& result, const string& filename) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) throw runtime_error("Не удалось создать файл: " + filename);
    outFile << result;
    cout << "Результат сохранён в '" << filename << "'\n";
}

void menu_rozkov() {
    cout << "\n=== Бифид / ADFGX ===\n";
    cout << "  1. Бифид — зашифровать\n";
    cout << "  2. Бифид — расшифровать\n";
    cout << "  3. ADFGX — зашифровать\n";
    cout << "  4. ADFGX — расшифровать\n";
    cout << "  0. Назад\n";

    int choice = input_int("Выбор: ", 0, 4);
    if (choice == 0) return;

    try {
        // Для ADFGX нужно ключевое слово
        string keyword;
        if (choice == 3 || choice == 4) {
            keyword = input_string("Ключевое слово: ");
        }

        cout << "Источник данных (1 — файл, 2 — консоль): ";
        int source = input_int("", 1, 2);
        string text = get_input_text(source == 1);

        string outfile = input_path("Имя файла для сохранения результата");

        string result;
        switch (choice) {
            case 1: result = encryptBifid(text);               break;
            case 2: result = decryptBifid(text);               break;
            case 3: result = encryptADFGX(text, keyword);      break;
            case 4: result = decryptADFGX(text, keyword);      break;
        }

        if (source == 2) {
            cout << "Результат: " << result << "\n";
        }
        save_result(result, outfile);

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }
}
