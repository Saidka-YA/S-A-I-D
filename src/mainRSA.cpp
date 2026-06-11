#include "RSAblock.h"
#include "RSAutilities.h"
#include <iostream>
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

    string keyPath = "rsaKey.bin";
    
    // Инициализация mpz переменных для ключей
    mpz_t n, e, d;
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    if (choice == 1)
    {
        // Получение адресов файлов для считывания и сохранения данных
        string inputPath, outputPath;
        cout << "Введите путь к файлу: ";
        getline(cin, inputPath);
        cout << "Введите путь для сохранения результата: ";
        getline(cin, outputPath);
        // Читаем и записываем данные в вектор data
        vector<uint8_t> data;
        if (!readFile(inputPath, data))
        {
            cerr << "Ошибка чтения файла\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
            return 1;
        }
        // Генерируем ключи и сохраняем их в файл
        cout << "Генерация ключей...\n";
        generateKeys(n, e, d);
        if (!saveKey(keyPath, n, e, d))
        {
            cerr << "Ошибка сохранения ключа\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
            return 1;
        }
        // Добавляем байты в конец данных чтобы были кратны 64 байтам
        Padding(data);
        // Шифруем данные и записываем в ciphertext
        cout << "Шифрование...\n";
        vector<uint8_t> ciphertext;
        encryptData(data, ciphertext, n, e);

        if (!writeFile(outputPath, ciphertext))
        {
            cerr << "Ошибка записи файла\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
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

        if (!loadKey(keyPath, n, e, d))
        {
            cerr << "Ошибка загрузки ключа\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
            return 1;
        }

        vector<uint8_t> ciphertext;
        if (!readFile(inputPath, ciphertext))
        {
            cerr << "Ошибка чтения файла\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
            return 1;
        }

        cout << "Расшифрование...\n";
        vector<uint8_t> plaintext;
        decryptData(ciphertext, plaintext, n, d);
        deletePadding(plaintext);

        if (!writeFile(outputPath, plaintext))
        {
            cerr << "Ошибка записи файла\n";
            mpz_clear(n);
            mpz_clear(e);
            mpz_clear(d);
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

        // Генерируем ключи, сохраняем
        cout << "Генерация ключей...\n";
        generateKeys(n, e, d);
        saveKey(keyPath, n, e, d);

        Padding(data);

        // Шифруем
        cout << "Шифрование...\n";
        vector<uint8_t> ciphertext;
        encryptData(data, ciphertext, n, e);

        cout << "\nЗашифрованный текст (hex):\n";
        cout << hexDisplay(ciphertext) << endl;

        // Расшифруем сразу для проверки
        cout << "\nРасшифрование...\n";
        vector<uint8_t> plaintext;
        decryptData(ciphertext, plaintext, n, d);
        deletePadding(plaintext);

        string decrypted(plaintext.begin(), plaintext.end());
        cout << "\nРасшифрованный текст:\n";
        cout << decrypted << endl;
    }

    else
    {
        cerr << "Неверный выбор\n";
        mpz_clear(n);
        mpz_clear(e);
        mpz_clear(d);
        return 1;
    }

    // Очистка mpz переменных
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);

    return 0;
}