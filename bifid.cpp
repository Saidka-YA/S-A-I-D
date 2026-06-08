#include "bifid.h"
#include "hex_utils.h"

#include <vector>
#include <stdexcept>
#include <cctype>
#include <utility>

using namespace std;

// Вспомогательные функции 
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

char coordsToHexDigit(int row, int col) {
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        throw invalid_argument("координаты вне диапазона 0-3");
    }
    int val = row * 4 + col;
    return (val < 10) ? ('0' + val) : ('A' + val - 10);
}

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

string decryptBifid(const string& ciphertext) {
    if (ciphertext.empty()) return "";

    string cleanHex;
    for (char c : ciphertext) {
        if (!isspace(static_cast<unsigned char>(c))) {
            cleanHex += toupper(static_cast<unsigned char>(c));
        }
    }

    if (cleanHex.empty()) return "";

    vector<int> flatCoords;
    flatCoords.reserve(cleanHex.length() * 2);
    for (char ch : cleanHex) {
        pair<int, int> coords = hexDigitToCoords(ch);
        flatCoords.push_back(coords.first);
        flatCoords.push_back(coords.second);
    }

    size_t half = flatCoords.size() / 2;
    
    string origHex;
    origHex.reserve(cleanHex.length());
    for (size_t i = 0; i < half; ++i) {
        origHex += coordsToHexDigit(flatCoords[i], flatCoords[half + i]);
    }

    string plaintext;
    plaintext.reserve(origHex.length() / 2);
    for (size_t i = 0; i < origHex.length(); i += 2) {
        plaintext += static_cast<char>(hexToByte(origHex.substr(i, 2)));
    }

    return plaintext;
}