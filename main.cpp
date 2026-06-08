#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "bifid.h"
#include "adfgx.h"
#include "console_utils.h"

using namespace std;

int main() {
    setupConsoleUTF8();
    
    try {
        cout << "   КРИПТОГРАФИЧЕСКИЕ ШИФРЫ (Бифид/ADFGX)\n\n";
        
        // ВЫБОР ШИФРА
        cout << "[0] Выберите шифр (1 - Бифид, 2 - ADFGX): ";
        string cipherStr;
        getline(cin, cipherStr);
        int cipher;
        try { cipher = stoi(cipherStr); } catch (...) { throw invalid_argument("Неверный формат выбора шифра"); }
        if (cipher != 1 && cipher != 2) throw invalid_argument("Шифр должен быть 1 или 2.");
        
        string cipherName = (cipher == 1) ? "БИФИД" : "ADFGX";
        cout << "\n>>> Выбран шифр: " << cipherName << " <<<\n\n";
        
        // ВВОД КЛЮЧЕВОГО СЛОВА (только для ADFGX)
        string cleanKey;
        if (cipher == 2) {
            cout << "[1] Ключевое слово: ";
            string keyword;
            getline(cin, keyword);
            
            for (size_t i = 0; i < keyword.length(); ++i) {
                unsigned char c = static_cast<unsigned char>(keyword[i]);
                if (!isspace(c)) {
                    cleanKey += keyword[i];
                }
            }
            if (cleanKey.empty()) {
                throw invalid_argument("Ключевое слово не может быть пустым");
            }
        }
        
        //  ВЫБОР РЕЖИМА И ИСТОЧНИКА ДАННЫХ
        cout << "[" << (cipher == 1 ? "1" : "2") << "] Режим (1 - Шифрование, 2 - Расшифровка): ";
        string modeStr;
        getline(cin, modeStr);
        int mode;
        try { mode = stoi(modeStr); } catch (...) { throw invalid_argument("Неверный формат режима"); }
        if (mode != 1 && mode != 2) throw invalid_argument("Режим должен быть 1 или 2.");
        
        cout << "[" << (cipher == 1 ? "2" : "3") << "] Источник данных (1 - Файл .txt, 2 - Ввод в консоль): ";
        string sourceStr;
        getline(cin, sourceStr);
        int source;
        try { source = stoi(sourceStr); } catch (...) { throw invalid_argument("Неверный формат источника"); }
        if (source != 1 && source != 2) throw invalid_argument("Источник должен быть 1 или 2");
        
        string textToProcess;
        string outputFileName;
        
        // ОБРАБОТКА ВВОДА (ФАЙЛ ИЛИ КОНСОЛЬ) 
        if (source == 1) {
            cout << "[" << (cipher == 1 ? "3" : "4") << "] Имя исходного файла (например, input.txt): ";
            string inputFileName;
            getline(cin, inputFileName);
            
            cout << "[" << (cipher == 1 ? "4" : "5") << "] Имя файла для сохранения результата (например, output.txt): ";
            getline(cin, outputFileName);
            
            ifstream inFile(inputFileName, ios::binary);
            if (!inFile) {
                throw runtime_error("Не удалось открыть файл для чтения: " + inputFileName);
            }
            
            stringstream buffer;
            buffer << inFile.rdbuf();
            textToProcess = buffer.str();
            inFile.close();
            
            if (textToProcess.empty()) {
                throw invalid_argument("Исходный файл пуст");
            }
        } 
        else {
            if (mode == 1) {
                cout << "[" << (cipher == 1 ? "3" : "4") << "] Введите ОТКРЫТЫЙ ТЕКСТ для шифрования: ";
            } else {
                cout << "[" << (cipher == 1 ? "3" : "4") << "] Введите ШИФРТЕКСТ для расшифровки: ";
            }
            getline(cin, textToProcess);
            if (textToProcess.empty()) {
                throw invalid_argument("Текст не может быть пустым");
            }
            
            cout << "[" << (cipher == 1 ? "4" : "5") << "] Имя нового файла для копирования результата (например, result.txt): ";
            getline(cin, outputFileName);
        }
        
        if (outputFileName.empty()) {
            throw invalid_argument("Имя файла для записи не может быть пустым");
        }
        
        // ЗАПУСК КРИПТОГРАФИЧЕСКОЙ ОПЕРАЦИИ 
        string result;
        cout << "\n";
        if (mode == 1) {
            if (cipher == 1) {
                result = encryptBifid(textToProcess);
            } else {
                result = encryptADFGX(textToProcess, cleanKey);
            }
            if (source == 2) {
                cout << "[ШИФРТЕКСТ В КОНСОЛИ]: " << result << "\n";
            }
        } else {
            if (cipher == 1) {
                result = decryptBifid(textToProcess);
            } else {
                result = decryptADFGX(textToProcess, cleanKey);
            }
            if (source == 2) {
                cout << "[ОТКРЫТЫЙ ТЕКСТ В КОНСОЛИ]: " << result << "\n";
            }
        }
        
        // СОХРАНЕНИЕ РЕЗУЛЬТАТА НА ДИСК 
        ofstream outFile(outputFileName, ios::binary);
        if (!outFile) {
            throw runtime_error("Не удалось создать файл для записи: " + outputFileName);
        }
        outFile << result;
        outFile.close();
        
        cout << "[УСПЕХ]: Результат успешно скопирован и сохранен в файл '" << outputFileName << "'\n";
        
    } catch (const exception& e) {
        cerr << "\n[ОШИБКА]: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
