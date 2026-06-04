#include "hill.h"
#include <string>
#include <filesystem>

namespace fs = filesystem;

int main() {
    vector<char32_t> alphabet = genalphabet();

    int choice;
    cout << "1 - Зашифровать\n2 - Расшифровать\n";
    cout << "Выберите действие: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // Выбор источника текста
        int source;
        cout << "1 - Ввести текст вручную\n2 - Прочитать из файла\n";
        cout << "Выберите источник текста: ";
        cin >> source;

        string text;
        if (source == 1) {
            cin.ignore();
            cout << "Введите текст: ";
            getline(cin, text);
        } else if (source == 2) {
            string filename;
            cout << "Введите путь к файлу: ";
            cin >> filename;
            cin.ignore();
            if (fs::is_directory(filename)) {
                cerr << "Ошибка: '" << filename << "' является директорией, укажите путь к файлу!\n";
                return 1;
            }
            ifstream fin(filename, ios::binary);
            if (!fin.is_open()) {
                cerr << "Ошибка: файл '" << filename << "' не найден!\n";
                return 1;
            }
            text.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
            fin.close();
            cout << "Текст загружен (" << text.size() << " байт)\n";
        } else {
            cerr << "Неверный выбор!\n";
            return 1;
        }

        // Генерируем ключ
        int n;
        cout << "Введите размер ключевой матрицы: ";
        cin >> n;
        cin.ignore();

        string keyword;
        cout << "Введите ключевое слово: ";
        getline(cin, keyword);

        Matrix K = keyFromWord(keyword, n, alphabet);
        if (K.empty()) return 1;

        // Сохраняем ключ
        string keyfile;
        cout << "Введите путь для сохранения ключа (или Enter чтобы пропустить): ";
        getline(cin, keyfile);
        if (!keyfile.empty()) {
            saveKey(K, keyfile);
            cout << "Ключ сохранён в '" << keyfile << "'\n";
        }

        // Шифруем
        size_t len = to_codes(text).size();
        string encrypted = encrypt(text, K, alphabet);
        cout << "Зашифрованный текст:\n" << encrypted << "\n";

        // Сохраняем зашифрованный текст
        string encfile;
        cout << "Введите путь для сохранения шифротекста (или Enter чтобы пропустить): ";
        getline(cin, encfile);
        if (!encfile.empty()) {
            ofstream fenc(encfile, ios::binary);
            if (!fenc.is_open()) {
                cerr << "Ошибка: не удалось создать файл '" << encfile << "'!\n";
                return 1;
            }
            fenc << len << "\n" << encrypted;
            fenc.close();
            cout << "Шифротекст сохранён в '" << encfile << "'\n";
        }

    } else if (choice == 2) {
        // Загружаем ключ
        string keyfile;
        cout << "Введите путь к файлу ключа: ";
        cin >> keyfile;
        cin.ignore();
        Matrix K = loadKey(keyfile);
        if (K.empty()) return 1;

        // Читаем зашифрованный текст
        string encfile;
        cout << "Введите путь к файлу шифротекста: ";
        cin >> encfile;
        cin.ignore();
        if (fs::is_directory(encfile)) {
            cerr << "Ошибка: '" << encfile << "' является директорией, укажите путь к файлу!\n";
            return 1;
        }
        ifstream fin(encfile, ios::binary);
        if (!fin.is_open()) {
            cerr << "Ошибка: файл '" << encfile << "' не найден!\n";
            return 1;
        }
        string encContent((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        fin.close();

        // Извлекаем длину и шифротекст
        size_t newline = encContent.find('\n');
        if (newline == string::npos) {
            cerr << "Ошибка: неверный формат файла шифротекста!\n";
            return 1;
        }
        size_t len = stoul(encContent.substr(0, newline));
        string encrypted = encContent.substr(newline + 1);

        // Расшифровываем
        string decrypted = decrypt(encrypted, K, alphabet, len);
        cout << "Расшифрованный текст:\n" << decrypted << "\n";

        // Сохраняем расшифрованный текст
        string decfile;
        cout << "Введите путь для сохранения расшифрованного текста (или Enter чтобы пропустить): ";
        getline(cin, decfile);
        if (!decfile.empty()) {
            ofstream fdec(decfile, ios::binary);
            if (!fdec.is_open()) {
                cerr << "Ошибка: не удалось создать файл '" << decfile << "'!\n";
                return 1;
            }
            fdec << decrypted;
            fdec.close();
            cout << "Расшифрованный текст сохранён в '" << decfile << "'\n";
        }

    } else {
        cerr << "Неверный выбор!\n";
        return 1;
    }

    return 0;
}
