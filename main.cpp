#include "hill.h"
#include "vernam.h"
#include "filef.h"
#include <string>
#include <filesystem>
#include <cstdint>
#include <fstream>

namespace fs = filesystem;
enum class Algorithm { HILL, VERNAM };
enum class Action { ENCRYPT, DECRYPT };
enum class Source { MANUAL, FILE };

int main() {
    // выбор алгоритма
    string input;
    Algorithm algo;
    cout << "Выберите алгоритм (hill / vernam): ";
    cin >> input;
    if (input == "hill" || input == "HILL") algo = Algorithm::HILL;
    else if (input == "vernam" || input == "VERNAM") algo = Algorithm::VERNAM;
    else { cerr << "Неверный выбор!\n"; return 1; }

    // выбор действия
    Action action;
    cout << "Выберите действие (encrypt / decrypt): ";
    cin >> input;
    if (input == "encrypt") action = Action::ENCRYPT;
    else if (input == "decrypt") action = Action::DECRYPT;
    else { cerr << "Неверный выбор!\n"; return 1; }

    if (action == Action::ENCRYPT) {
        string text;
        if (algo == Algorithm::HILL) {
            Source source;
            cout << "Выберите источник текста (manual / file): ";
            cin >> input;
            if (input == "manual") source = Source::MANUAL;
            else if (input == "file") source = Source::FILE;
            else { cerr << "Неверный выбор!\n"; return 1; }

            if (source == Source::MANUAL) {
                cin.ignore();
                cout << "Введите текст: ";
                getline(cin, text);
            } else {
                string filename;
                cout << "Введите путь к файлу: ";
                cin >> filename;
                cin.ignore();
                if (!readFile(filename, text)) return 1;
                cout << "Текст загружен (" << text.size() << " байт)\n";
            }
        } else {
            string filename;
            cout << "Введите путь к файлу: ";
            cin >> filename;
            cin.ignore();
            if (!readFile(filename, text)) return 1;
            cout << "Файл загружен (" << text.size() << " байт)\n";
        }

        string encrypted;
        if (algo == Algorithm::HILL) {
            int n;
            cout << "Введите размер ключевой матрицы: ";
            cin >> n;
            cin.ignore();
            string keyword;
            cout << "Введите ключевое слово: ";
            getline(cin, keyword);
            
            Matrix K = keyFromWord(keyword, n);
            if (K.empty()) return 1;

            string keyfile;
            cout << "Введите путь для сохранения ключа (или Enter чтобы пропустить): ";
            getline(cin, keyfile);
            if (!keyfile.empty()) {
                saveKey(K, keyfile);
                cout << "Ключ сохранён в '" << keyfile << "'\n";
            }

            size_t len = text.size(); // ИСПРАВЛЕНО: теперь это просто размер строки в байтах
            encrypted = hillEncrypt(text, K);
            
            string encfile;
            cout << "Введите путь для сохранения шифротекста (или Enter чтобы пропустить): ";
            getline(cin, encfile);
            if (!encfile.empty()) {
                // ИСПОЛЬЗУЕМ БИНАРНЫЙ ЗАГОЛОВОК (4 байта) вместо текстового "\n".
                // Это критически важно, так как в бинарных данных символ \n (0x0A) 
                // может встретиться случайно и сломать поиск разделителя при дешифровании.
                ofstream f(encfile, ios::binary);
                if (f) {
                    uint32_t len32 = static_cast<uint32_t>(len);
                    f.write(reinterpret_cast<const char*>(&len32), sizeof(uint32_t));
                    f.write(encrypted.data(), encrypted.size());
                    f.close();
                    cout << "Шифротекст сохранён\n";
                }
            }
            cout << "Зашифровано байт: " << encrypted.size() << "\n";
        } else {
            vector<uint8_t> key = vernamKeyFromText(text);
            string keyfile;
            cout << "Введите путь для сохранения ключа (или Enter чтобы пропустить): ";
            getline(cin, keyfile);
            if (!keyfile.empty()) {
                saveVernamKey(key, keyfile);
                cout << "Ключ сохранён в '" << keyfile << "'\n";
            }

            encrypted = vernamEncrypt(text, key);
            string encfile;
            cout << "Введите путь для сохранения шифротекста (или Enter чтобы пропустить): ";
            getline(cin, encfile);
            if (!encfile.empty()) {
                writeFile(encfile, encrypted);
            }
            cout << "Шифрование завершено (" << encrypted.size() << " байт)\n";
        }
    } else {
        // дешифрование
        string keyfile;
        cout << "Введите путь к файлу ключа: ";
        cin >> keyfile;
        cin.ignore();
        string encfile;
        cout << "Введите путь к файлу шифротекста: ";
        cin >> encfile;
        cin.ignore();

        string decrypted;
        if (algo == Algorithm::HILL) {
            Matrix K = loadKey(keyfile);
            if (K.empty()) return 1;

            // Читаем бинарный заголовок (первые 4 байта = исходная длина)
            ifstream f(encfile, ios::binary);
            if (!f) {
                cerr << "Ошибка: не удалось открыть файл шифротекста!\n";
                return 1;
            }
            uint32_t len = 0;
            f.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));
            
            // Читаем остаток файла как сам шифротекст
            string enc((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
            f.close();

            decrypted = hillDecrypt(enc, K, len);
            cout << "Расшифровано байт: " << decrypted.size() << "\n";
        } else {
            vector<uint8_t> key = loadVernamKey(keyfile);
            if (key.empty()) return 1;
            
            string encContent;
            if (!readFile(encfile, encContent)) return 1;
            
            decrypted = vernamDecrypt(encContent, key);
            cout << "Расшифровка завершена (" << decrypted.size() << " байт)\n";
        }

        string decfile;
        cout << "Введите путь для сохранения результата (или Enter чтобы пропустить): ";
        getline(cin, decfile);
        if (!decfile.empty()) {
            writeFile(decfile, decrypted);
        }
    }
    return 0;
}