#include "hill.h"
#include "vernam.h"
#include "filef.h"
#include <string>
#include <filesystem>

namespace fs = filesystem;

enum class Algorithm { HILL, VERNAM };
enum class Action { ENCRYPT, DECRYPT };
enum class Source { MANUAL, FILE };

int main() {
    vector<char32_t> alphabet = genalphabet();

    // Выбор алгоритма
    string input;
    Algorithm algo;
    cout << "Выберите алгоритм (hill / vernam): ";
    cin >> input;
    if (input == "hill" || input == "HILL") algo = Algorithm::HILL;
    else if (input == "vernam" || input == "VERNAM") algo = Algorithm::VERNAM;
    else { cerr << "Неверный выбор!\n"; return 1; }

    // Выбор действия
    Action action;
    cout << "Выберите действие (encrypt / decrypt): ";
    cin >> input;
    if (input == "encrypt") action = Action::ENCRYPT;
    else if (input == "decrypt") action = Action::DECRYPT;
    else { cerr << "Неверный выбор!\n"; return 1; }

    if (action == Action::ENCRYPT) {
        // Выбор источника текста
        Source source;
        cout << "Выберите источник текста (manual / file): ";
        cin >> input;
        if (input == "manual") source = Source::MANUAL;
        else if (input == "file") source = Source::FILE;
        else { cerr << "Неверный выбор!\n"; return 1; }

        string text;
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

        string encrypted;

        if (algo == Algorithm::HILL) {
            int n;
            cout << "Введите размер ключевой матрицы: ";
            cin >> n;
            cin.ignore();

            string keyword;
            cout << "Введите ключевое слово: ";
            getline(cin, keyword);

            Matrix K = keyFromWord(keyword, n, alphabet);
            if (K.empty()) return 1;

            string keyfile;
            cout << "Введите путь для сохранения ключа (или Enter чтобы пропустить): ";
            getline(cin, keyfile);
            if (!keyfile.empty()) {
                saveKey(K, keyfile);
                cout << "Ключ сохранён в '" << keyfile << "'\n";
            }

            size_t len = to_codes(text).size();
            encrypted = hillEncrypt(text, K, alphabet);

            string encfile;
            cout << "Введите путь для сохранения шифротекста (или Enter чтобы пропустить): ";
            getline(cin, encfile);
            if (!encfile.empty())
                writeFile(encfile, to_string(len) + "\n" + encrypted);

        } else {
            // Вернам — ключ генерируется автоматически
            cin.ignore();
            vector<int> key = vernamKeyFromText(text, alphabet);

            string keyfile;
            cout << "Введите путь для сохранения ключа (или Enter чтобы пропустить): ";
            getline(cin, keyfile);
            if (!keyfile.empty()) {
                saveVernamKey(key, keyfile);
                cout << "Ключ сохранён в '" << keyfile << "'\n";
            }

            encrypted = vernamEncrypt(text, key, alphabet);

            string encfile;
            cout << "Введите путь для сохранения шифротекста (или Enter чтобы пропустить): ";
            getline(cin, encfile);
            if (!encfile.empty())
                writeFile(encfile, encrypted);
        }

        cout << "Зашифрованный текст:\n" << encrypted << "\n";

    } else {
        // Дешифрование
        string keyfile;
        cout << "Введите путь к файлу ключа: ";
        cin >> keyfile;
        cin.ignore();

        string encfile;
        cout << "Введите путь к файлу шифротекста: ";
        cin >> encfile;
        cin.ignore();

        string encContent;
        if (!readFile(encfile, encContent)) return 1;

        string decrypted;

        if (algo == Algorithm::HILL) {
            Matrix K = loadKey(keyfile);
            if (K.empty()) return 1;

            size_t newline = encContent.find('\n');
            if (newline == string::npos) {
                cerr << "Ошибка: неверный формат файла шифротекста!\n";
                return 1;
            }
            size_t len = stoul(encContent.substr(0, newline));
            string encrypted = encContent.substr(newline + 1);
            decrypted = hillDecrypt(encrypted, K, alphabet, len);

        } else {
            vector<int> key = loadVernamKey(keyfile);
            if (key.empty()) return 1;
            decrypted = vernamDecrypt(encContent, key, alphabet);
        }

        cout << "Расшифрованный текст:\n" << decrypted << "\n";

        string decfile;
        cout << "Введите путь для сохранения расшифрованного текста (или Enter чтобы пропустить): ";
        getline(cin, decfile);
        if (!decfile.empty())
            writeFile(decfile, decrypted);
    }

    return 0;
}
