#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "bifid.h"
#include "console_utils.h"

using namespace std;

int main() {
    setupConsoleUTF8();

    try {
        cout << "ШИФР БИФИДА\n\n";

        cout << "[1] Режим работы (1 - Шифрование, 2 - Расшифровка): ";
        string modeStr;
        getline(cin, modeStr);
        int mode;
        try { mode = stoi(modeStr); } catch (...) { throw invalid_argument("Неверный формат режима."); }
        if (mode != 1 && mode != 2) throw invalid_argument("Режим должен быть 1 или 2.");

        cout << "[2] Источник данных (1 - Файл .txt, 2 - Ввод в консоль): ";
        string sourceStr;
        getline(cin, sourceStr);
        int source;
        try { source = stoi(sourceStr); } catch (...) { throw invalid_argument("Неверный формат источника."); }
        if (source != 1 && source != 2) throw invalid_argument("Источник должен быть 1 или 2.");

        string textToProcess;
        string outputFileName;

        if (source == 1) {
            cout << "[3] Имя исходного файла (например, input.txt): ";
            string inputFileName;
            getline(cin, inputFileName);

            cout << "[4] Имя файла для сохранения результата (например, output.txt): ";
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
                throw invalid_argument("Исходный файл пуст.");
            }
        } 
        else {
            if (mode == 1) {
                cout << "[3] Введите ОТКРЫТЫЙ ТЕКСТ (можно русские буквы): ";
            } else {
                cout << "[3] Введите ШИФРТЕКСТ (Hex-строку): ";
            }
            getline(cin, textToProcess);
            if (textToProcess.empty()) {
                throw invalid_argument("Текст не может быть пустым.");
            }

            cout << "[4] Имя нового файла для копирования результата (например, result.txt): ";
            getline(cin, outputFileName);
        }

        if (outputFileName.empty()) {
            throw invalid_argument("Имя файла для записи не может быть пустым.");
        }

        string result;
        cout << "\n";
        if (mode == 1) {
            result = encryptBifid(textToProcess);
            if (source == 2) {
                cout << "[ШИФРТЕКСТ В КОНСОЛИ]: " << result << "\n";
            }
        } else {
            result = decryptBifid(textToProcess);
            if (source == 2) {
                cout << "[ОТКРЫТЫЙ ТЕКСТ В КОНСОЛИ]: " << result << "\n";
            }
        }

        ofstream outFile(outputFileName, ios::binary);
        if (!outFile) {
            throw runtime_error("Не удалось создать файл для записи: " + outputFileName);
        }
        outFile << result;
        outFile.close();

        cout << "[УСПЕХ]: Результат успешно сохранен в файл '" << outputFileName << "'\n";

    } catch (const exception& e) {
        cerr << "\n[ОШИБКА]: " << e.what() << "\n";
        return 1;
    }

    return 0;
}