#include "hill.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    vector<char32_t> alphabet = genalphabet();
    int m = alphabet.size();

    // Ввод размера матрицы
    int n;
    cout << "Введите размер ключевой матрицы: ";
    cin >> n;

    // Ввод ключевого слова и генерация матрицы
    cin.ignore(); // очищаем буфер после cin >>
    string keyword;
    cout << "Введите ключевое слово (желательно не менее " << n * n << " символов): ";
    getline(cin, keyword);

    Matrix K = keyFromWord(keyword, n, alphabet);
    if (K.empty()) {
        cerr << "Программа завершена.\n";
        return 1;
    }

    // Ввод текста
    string text;
    cout << "Введите текст: ";
    getline(cin, text);

    size_t len = to_codes(text).size();
    // Шифрование
    string encrypted = encrypt(text, K, alphabet);
    cout << "Зашифрованный текст: " << encrypted << "\n";

    // Дешифрование
    string decrypted = decrypt(encrypted, K, alphabet, len);
    cout << "Расшифрованный текст: " << decrypted << "\n";

    if (text == decrypted) cout << "\nТексты совпадают\n";
    else cout << "\nТексты не совпадают\n";

    return 0;
}
