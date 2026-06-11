#include "bifid.h"
#include "hex_utils.h"
#include <vector>
#include <stdexcept>
#include <cctype>
#include <utility>

using namespace std;

// Шифрование данных по алгоритму Бифида черезHex-представление байт
string encryptBifid(const string& plaintext) {
    if (plaintext.empty()) return "";
    string hexStr;
    hexStr.reserve(plaintext.length() * 2);
    for (char c : plaintext) {
        hexStr += byteToHex(static_cast<unsigned char>(c));
    }
    vector<int> rows, cols;
    rows.reserve(hexStr.length());
    cols.reserve(hexStr.length());
    for (char ch : hexStr) {
        pair<int, int> coords = hexDigitToCoords(ch);
        rows.push_back(coords.first);
        cols.push_back(coords.second);
    }
    vector<int> combined = rows;
    combined.insert(combined.end(), cols.begin(), cols.end());
    string cipherHex;
    cipherHex.reserve(hexStr.length());
    for (size_t i = 0; i < combined.size(); i += 2) {
        cipherHex += coordsToHexDigit(combined[i], combined[i + 1]);
    }
    return cipherHex;
}

// Дешифрование данных по алгоритму Бифида
string decryptBifid(const string& ciphertext) {
    if (ciphertext.empty()) return "";
    string cleanHex;
    for (char c : ciphertext) {
        if (!isspace(static_cast<unsigned char>(c))) {
            cleanHex += toupper(static_cast<unsigned char>(c));
        }
    }
    if (cleanHex.empty()) return "";
    
    // Проверка четности длины строки для корректного разбиения на байты
    if (cleanHex.length() % 2 != 0) {
        throw invalid_argument("Нечетная длина последовательности hex-символов");
    }

    vector<int> flatCoords;
    flatCoords.reserve(cleanHex.length() * 2);
    for (char ch : cleanHex) {
        pair<int, int> coords = hexDigitToCoords(ch);
        flatCoords.push_back(coords.first);
        flatCoords.push_back(coords.second);
    }
    size_t half = flatCoords.size() / 2;
    
    string resultHex;
    resultHex.reserve(cleanHex.length());
    for (size_t i = 0; i < half; ++i) {
        resultHex += coordsToHexDigit(flatCoords[i], flatCoords[half + i]);
    }
    
    string result;
    result.reserve(resultHex.length() / 2);
    for (size_t i = 0; i < resultHex.length(); i += 2) {
        result += static_cast<char>(hexToByte(resultHex.substr(i, 2)));
    }
    return result;
}