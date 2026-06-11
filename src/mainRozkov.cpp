#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "bifid.h"
#include "adfgx.h"
#include "console_utils.h"

using namespace std;

// Перечисление для выбора криптосистемы в главном меню
enum class CipherSelection {
    Bifid = 1,
    Adfgx = 2
};

// Валидация и чтение целого числа из консоли
int safeReadInt(const string& prompt, int minVal, int maxVal) {
    string input;
    while (true) {
        cout << prompt;
        if (!getline(cin, input)) {
            cin.clear();
            continue;
        }
        
        input.erase(0, input.find_first_not_of(" \t\r\n"));
        input.erase(input.find_last_not_of(" \t\r\n") + 1);
        
        if (input.empty()) {
            cout << "Ошибка: ввод не должен быть пустым.\n";
            continue;
        }
        
        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(static_cast<unsigned char>(c))) {
                isNumber = false;
                break;
            }
        }
        
        if (!isNumber) {
            cout << "Ошибка: допускаются только цифры.\n";
            continue;
        }
        
        stringstream ss(input);
        int val;
        if (ss >> val) {
            if (val >= minVal && val <= maxVal) {
                return val;
            }
        }
        cout << "Ошибка: число должно быть в диапазоне от " << minVal << " до " << maxVal << ".\n";
    }
}

// Чтение непустой текстовой строки
string safeReadNonEmptyString(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        if (!getline(cin, input)) {
            cin.clear();
            continue;
        }
        if (input.empty()) {
            cout << "Ошибка: строка не должна быть пустой.\n";
            continue;
        }
        return input;
    }
}

// Чтение файла с проверкой на доступность
string safeReadTextFromFile() {
    while (true) {
        string path = safeReadNonEmptyString("  Путь к входному файлу: ");
        ifstream f(path, ios::binary);
        if (!f) {
            cout << "Ошибка: не удалось открыть файл '" << path << "'.\n";
            continue;
        }
        
        stringstream buf;
        buf << f.rdbuf();
        string text = buf.str();
        f.close();
        
        if (text.empty()) {
            cout << "Ошибка: выбранный файл пуст.\n";
            continue;
        }
        return text;
    }
}

int main() {
    setupConsoleUTF8();
    
    cout << "   КРИПТОГРАФИЧЕСКИЕ ШИФРЫ (Бифид / ADFGX)\n\n";
    
    // Безопасное чтение выбора алгоритма и приведение к enum class
    int cipherSelectionInt = safeReadInt("[0] Выберите шифр (1 - Бифид, 2 - ADFGX): ", 1, 2);
    CipherSelection chosenCipher = static_cast<CipherSelection>(cipherSelectionInt);
    
    string cipherName = (chosenCipher == CipherSelection::Bifid) ? "БИФИД" : "ADFGX";
    cout << "\n>>> Выбран шифр: " << cipherName << " <<<\n\n";
    
    // Чтение ключа (актуально только для алгоритма ADFGX)
    string cleanKey;
    if (chosenCipher == CipherSelection::Adfgx) {
        while (true) {
            string rawKey = safeReadNonEmptyString("[1] Ключевое слово (английские буквы): ");
            cleanKey = "";
            for (char c : rawKey) {
                if (isalpha(static_cast<unsigned char>(c))) {
                    cleanKey += toupper(static_cast<unsigned char>(c));
                }
            }
            if (cleanKey.empty()) {
                cout << "Ошибка: ключевое слово должно содержать латинские буквы.\n";
                continue;
            }
            break;
        }
        cout << "  Используемый ключ: " << cleanKey << "\n\n";
    }
    
    // Чтение режима работы
    int modeInt = safeReadInt("[2] Выберите режим (1 - Зашифровать, 2 - Расшифровать): ", 1, 2);
    
    // Источник данных
    int source = safeReadInt("[3] Источник данных (1 - Файл, 2 - Консоль): ", 1, 2);
    
    string textToProcess;
    if (source == 1) {
        textToProcess = safeReadTextFromFile();
    } else {
        textToProcess = safeReadNonEmptyString("  Введите текст для обработки: ");
    }
    
    string outputFileName = safeReadNonEmptyString("\n[4] Имя файла для записи результата: ");
    string result;
    
    try {
        // Управление операциями с использованием строго типизированных перечислений
        switch (chosenCipher) {
            case CipherSelection::Bifid: {
                BifidMode bMode = static_cast<BifidMode>(modeInt);
                switch (bMode) {
                    case BifidMode::Encrypt:
                        result = encryptBifid(textToProcess);
                        break;
                    case BifidMode::Decrypt:
                        result = decryptBifid(textToProcess);
                        break;
                }
                break;
            }
            case CipherSelection::Adfgx: {
                AdfgxMode aMode = static_cast<AdfgxMode>(modeInt);
                switch (aMode) {
                    case AdfgxMode::Encrypt:
                        result = encryptADFGX(textToProcess, cleanKey);
                        break;
                    case AdfgxMode::Decrypt:
                        result = decryptADFGX(textToProcess, cleanKey);
                        break;
                }
                break;
            }
        }

        if (source == 2) {
            cout << "\n[РЕЗУЛЬТАТ В КОНСОЛИ]: " << result << "\n";
        }
        
        ofstream outFile(outputFileName, ios::binary);
        if (!outFile) {
            cout << "Ошибка: не удалось создать файл для записи: " << outputFileName << "\n";
            return 1;
        }
        outFile << result;
        outFile.close();
        cout << "Результат успешно сохранен в файл '" << outputFileName << "'\n";
        
    } catch (const exception& e) {
        cout << "\nОшибка выполнения алгоритма: " << e.what() << "\n";
    }
    
    return 0;
}