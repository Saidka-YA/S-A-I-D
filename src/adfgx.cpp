#include "adfgx.h"
#include "hex_utils.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <iostream>

using namespace std;

// Символы осей для формирования пар координат матрицы
const char COORD_LABELS[] = {'A', 'D', 'F', 'G', 'X'};

// Преобразование входного текста в последовательность символов ADFGX через Hex
string fractionate(const string& text) {
    string result;
    result.reserve(text.length() * 4);
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char byte = static_cast<unsigned char>(text[i]);
        string hexStr = byteToHex(byte);
        for (size_t j = 0; j < hexStr.length(); ++j) {
            pair<int, int> coords = hexDigitToCoords(hexStr[j]);
            result += COORD_LABELS[coords.first];  
            result += COORD_LABELS[coords.second]; 
        }
    }
    return result;
}

// Обратное преобразование символов ADFGX в исходные байты
string defractionate(const string& fracText) {
    string filteredFrac;
    for (char c : fracText) {
        char upC = toupper(static_cast<unsigned char>(c));
        if (upC == 'A' || upC == 'D' || upC == 'F' || upC == 'G' || upC == 'X') {
            filteredFrac += upC;
        }
    }

    if (filteredFrac.empty()) {
        throw invalid_argument("Входные данные не содержат алфавита ADFGX");
    }

    if (filteredFrac.length() % 4 != 0) {
        throw invalid_argument("Длина последовательности не кратна 4");
    }

    string hexStr;
    hexStr.reserve(filteredFrac.length() / 2);
    for (size_t i = 0; i < filteredFrac.length(); i += 2) {
        char rowChar = filteredFrac[i];
        char colChar = filteredFrac[i+1];
        
        int r = -1, c = -1;
        for (int k = 0; k < 5; ++k) {
            if (COORD_LABELS[k] == rowChar) r = k;
            if (COORD_LABELS[k] == colChar) c = k;
        }
        
        if (r == -1 || c == -1) {
            throw invalid_argument("Некорректный символ в последовательности координат");
        }
        hexStr += coordsToHexDigit(r, c);
    }

    string result;
    result.reserve(hexStr.length() / 2);
    for (size_t i = 0; i < hexStr.length(); i += 2) {
        result += static_cast<char>(hexToByte(hexStr.substr(i, 2)));
    }
    return result;
}

// Выполнение столбцовой перестановки
string columnarTransposition(const string& text, const string& keyword, bool encrypt) {
    int kLen = keyword.length();
    if (kLen == 0) return text;

    vector<pair<char, int>> keyOrder(kLen);
    for (int i = 0; i < kLen; ++i) {
        keyOrder[i] = make_pair(keyword[i], i);
    }
    stable_sort(keyOrder.begin(), keyOrder.end(), [](const pair<char, int>& a, const pair<char, int>& b) {
        return a.first < b.first;
    });

    int L = text.length();
    int numRows = (L + kLen - 1) / kLen;

    if (encrypt) {
        string paddedText = text;
        while ((int)paddedText.length() < numRows * kLen) {
            paddedText += 'X'; 
        }

        vector<string> table(numRows, string(kLen, ' '));
        int idx = 0;
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < kLen; ++c) {
                table[r][c] = paddedText[idx++];
            }
        }

        string result;
        for (int i = 0; i < kLen; ++i) {
            int colIdx = keyOrder[i].second;
            for (int r = 0; r < numRows; ++r) {
                result += table[r][colIdx];
            }
        }
        return result;
    } else {
        if (L % kLen != 0) {
            throw invalid_argument("Размер шифртекста не соответствует матрице ключа");
        }

        vector<string> cols(kLen, string(numRows, ' '));
        int pos = 0;
        for (int sortedIdx = 0; sortedIdx < kLen; ++sortedIdx) {
            int origCol = keyOrder[sortedIdx].second;
            for (int r = 0; r < numRows; ++r) {
                if (pos >= L) throw length_error("Выход за границы данных при чтении столбцов");
                cols[origCol][r] = text[pos++];
            }
        }

        string result;
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < kLen; ++c) {
                result += cols[c][r];
            }
        }
        return result;
    }
}

string encryptADFGX(const string& plaintext, const string& keyword) {
    if (plaintext.empty()) return "";
    string frac = fractionate(plaintext);

    // Дополняем дробную последовательность парами 'AA' (= hex 0, байт 0x00),
    // чтобы после столбцовой перестановки общая длина оставалась кратной 4.
    // Перестановка не меняет общую длину, поэтому достаточно сделать длину
    // frac кратной НОК(4, длина ключа).
    int kLen = (int)keyword.length();
    int blockLen = (kLen > 0) ? (int)std::lcm(4, kLen) : 4;
    while ((int)frac.length() % blockLen != 0) {
        frac += "AA";
    }

    return columnarTransposition(frac, keyword, true);
}

string decryptADFGX(const string& ciphertext, const string& keyword) {
    if (ciphertext.empty()) return "";
    
    string clean;
    for (char c : ciphertext) {
        if (!isspace(static_cast<unsigned char>(c))) {
            clean += toupper(static_cast<unsigned char>(c));
        }
    }
    
    string transposed = columnarTransposition(clean, keyword, false);
    string result = defractionate(transposed);

    // Убираем хвостовые нулевые байты, добавленные при выравнивании длины
    while (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }
    return result;
}