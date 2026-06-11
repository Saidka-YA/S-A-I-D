#include "CFBmode.h"
#include "AES128.h"
#include "utilities.h"
using namespace std;

int main()
{
    cout << "1 - Зашифровать файл\n";
    cout << "2 - Расшифровать файл\n";
    cout << "3 - Зашифровать текст (консоль)\n";
    cout << "Выбор: ";

    int choice;
    cin >> choice;
    cin.ignore();

    string keyPath = "aesKey.bin" ;
    uint8_t key[16];
    uint8_t expandedKey[176];

    if (choice == 1)
    {
        // Получение адресов файлов для считывания и сохранения данных
        string inputPath, outputPath;
        cout << "Введите путь к файлу: ";
        getline(cin, inputPath);
        cout << "Введите путь для сохранения результата: ";
        getline(cin, outputPath);
        // Читаем и записываем данные в вектор date
        vector<uint8_t> data;
        if (!readFile(inputPath, data))
        {
            cerr << "Ошибка чтения файла\n";
            return 1;
        }
        // Генерируем ключ и сохраняем его в файл
        generateKey(key);
        if (!saveKey(keyPath, key))
        {
            cerr << "Ошибка сохранения ключа\n";
            return 1;
        }
        expandRoundKey(key, expandedKey);
        // Добавляем байты в конец данных чтобы были кратны 16 байтам
        Padding(data);
        // Шифруем данные и записываем в ciphertext
        vector<uint8_t> ciphertext;
        encryptCFB(data, ciphertext, expandedKey);

        if (!writeFile(outputPath, ciphertext))
        {
            cerr << "Ошибка записи файла\n";
            return 1;
        }

        cout << "Файл зашифрован. Ключ сохранён в " << keyPath << endl;
    }

    else if (choice == 2)
    {
        // Получаем адреса файлов откуда читать и куда записывать
        string inputPath, outputPath;
        cout << "Введите путь к файлу: ";
        getline(cin, inputPath);
        cout << "Введите путь для сохранения результата: ";
        getline(cin, outputPath);

        if (!loadKey(keyPath, key))
        {
            cerr << "Ошибка загрузки ключа\n";
            return 1;
        }
        expandRoundKey(key, expandedKey);

        vector<uint8_t> ciphertext;
        if (!readFile(inputPath, ciphertext))
        {
            cerr << "Ошибка чтения файла\n";
            return 1;
        }

        vector<uint8_t> plaintext;
        decryptCFB(ciphertext, plaintext, expandedKey);
        deletePadding(plaintext);

        if (!writeFile(outputPath, plaintext))
        {
            cerr << "Ошибка записи файла\n";
            return 1;
        }
        cout << "Файл расшифрован.\n";
    }

    else if (choice == 3)
    {
        cout << "Ввод текста с переносами строк будет обработан неверно!\n";
        cout << "Для расшифрования/дешифрования большого текста "
                    << "используйте режим расшифрования/дефишрования файлов\n";
        cout << "Введите текст для шифрования: ";
        string text;
        getline(cin, text);

        // Текст -> байты
        vector<uint8_t> data(text.begin(), text.end());

        // Генерируем ключ, сохраняем
        generateKey(key);
        saveKey(keyPath, key);
        expandRoundKey(key, expandedKey);

        Padding(data);

        // Шифруем
        vector<uint8_t> ciphertext;
        encryptCFB(data, ciphertext, expandedKey);

        cout << "\nЗашифрованный текст (hex):\n";
        cout << hexDisplay(ciphertext) << endl;

        // Расшифруем сразу для проверки
        vector<uint8_t> plaintext;
        decryptCFB(ciphertext, plaintext, expandedKey);
        deletePadding(plaintext);

        string decrypted(plaintext.begin(), plaintext.end());
        cout << "\nРасшифрованный текст:\n";
        cout << decrypted << endl;
    }

    else
    {
        cerr << "Неверный выбор\n";
        return 1;
    }

    return 0;
}