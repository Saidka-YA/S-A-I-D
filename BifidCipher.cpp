#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <clocale>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

using namespace std;

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

// Перевод байта в Hex-строку
string byteToHex(unsigned char byte) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(2) << static_cast<int>(byte);
    return ss.str();
}

// Перевод Hex-строки в байт
unsigned char hexToByte(const string& hex) {
    if (hex.length() != 2) {
        throw invalid_argument("hex-строка должна иметь длину 2");
    }
    
    auto hexVal = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    };
    
    int high = hexVal(hex[0]);
    int low = hexVal(hex[1]);
    
    if (high == -1 || low == -1) {
        throw invalid_argument(string("некорректная hex-пара: ") + hex);
    }
    
    return static_cast<unsigned char>((high << 4) | low);
}

// Получение координат в сетке 4x4 для Hex-символа
pair<int, int> hexDigitToCoords(char hexDigit) {
    int val;
    if (hexDigit >= '0' && hexDigit <= '9') {
        val = hexDigit - '0';
    } else if (hexDigit >= 'A' && hexDigit <= 'F') {
        val = hexDigit - 'A' + 10;
    } else if (hexDigit >= 'a' && hexDigit <= 'f') {
        val = hexDigit - 'a' + 10;
    } else {
        throw invalid_argument(string("Недопустимый hex-символ: '") + hexDigit + "'");
    }
    return make_pair(val / 4, val % 4);
}

// Получение Hex-символа из координат сетки 4x4
char coordsToHexDigit(int row, int col) {
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        throw invalid_argument("координаты вне диапазона 0-3");
    }
    int val = row * 4 + col;
    return (val < 10) ? ('0' + val) : ('A' + val - 10);
}

// Шифрование Бифида 
string encryptBifid(const string& plaintext) {
    if (plaintext.empty()) return "";

    // 1. Переводим исходный текст (включая русские UTF-8 чары) в Hex
    string hexStr;
    hexStr.reserve(plaintext.length() * 2);
    for (char c : plaintext) {
        hexStr += byteToHex(static_cast<unsigned char>(c));
    }

    // 2. Извлекаем координаты строк и столбцов для каждой hex-цифры
    vector<int> rows, cols;
    rows.reserve(hexStr.length());
    cols.reserve(hexStr.length());
    for (char ch : hexStr) {
        pair<int, int> coords = hexDigitToCoords(ch);
        rows.push_back(coords.first);
        cols.push_back(coords.second);
    }

    // 3. Логика Бифида: объединяем строки и столбцы
    vector<int> combined = rows;
    combined.insert(combined.end(), cols.begin(), cols.end());

    // 4. Формируем зашифрованную Hex-строку
    string cipherHex;
    cipherHex.reserve(hexStr.length());
    for (size_t i = 0; i < combined.size(); i += 2) {
        cipherHex += coordsToHexDigit(combined[i], combined[i + 1]);
    }

    return cipherHex; // Возвращаем как текст (без упаковки в сырые байты)
}

// Расшифровка Бифида (принимает Hex-строку, возвращает исходный UTF-8 текст)
string decryptBifid(const string& ciphertext) {
    if (ciphertext.empty()) return "";

    // Очищаем входную строку от возможных пробелов или переносов строк
    string cleanHex;
    for (char c : ciphertext) {
        if (!isspace(static_cast<unsigned char>(c))) {
            cleanHex += toupper(static_cast<unsigned char>(c));
        }
    }

    if (cleanHex.empty()) return "";

    // 1. Извлекаем координаты из зашифрованной Hex-строки
    vector<int> flatCoords;
    flatCoords.reserve(cleanHex.length() * 2);
    for (char ch : cleanHex) {
        pair<int, int> coords = hexDigitToCoords(ch);
        flatCoords.push_back(coords.first);
        flatCoords.push_back(coords.second);
    }

    // 2. Разделяем массив координат обратно на две половины
    size_t half = flatCoords.size() / 2;
    
    // 3. Восстанавливаем оригинальную Hex-строку исходного текста
    string origHex;
    origHex.reserve(cleanHex.length());
    for (size_t i = 0; i < half; ++i) {
        origHex += coordsToHexDigit(flatCoords[i], flatCoords[half + i]);
    }

    // 4. Переводим Hex-строку обратно в нормальные байты (символы UTF-8)
    string plaintext;
    plaintext.reserve(origHex.length() / 2);
    for (size_t i = 0; i < origHex.length(); i += 2) {
        plaintext += static_cast<char>(hexToByte(origHex.substr(i, 2)));
    }

    return plaintext;
}

void setupConsoleUTF8() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#else
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
}

int main() {
    setupConsoleUTF8();

    try {
        cout << "ШИФР БИФИДА (БЕЗОПАСНАЯ HEX/UTF-8 ВЕРСИЯ)\n\n";

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
            // Файловый ввод
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
            // Консольный ввод
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

        // Выполнение криптографической операции
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

        // Запись результата в файл
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
