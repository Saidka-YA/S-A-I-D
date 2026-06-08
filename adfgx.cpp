#include "adfgx.h"
#include "hex_utils.h"

#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

using namespace std;

// Символы координат для осей матрицы ADFGX
const char COORD_LABELS[] = {'A', 'D', 'F', 'G', 'X'};

// ФРАКЦИОНИРОВАНИЕ: Перевод текста в Hex, а затем в буквы ADFGX по координатам
string fractionate(const string& text) {
    string result;
    result.reserve(text.length() * 4);
    
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char byte = static_cast<unsigned char>(text[i]);
        string hexStr = byteToHex(byte);
        for (size_t j = 0; j < hexStr.length(); ++j) {
            pair<int, int> coords = hexDigitToCoords(hexStr[j]);
            result += COORD_LABELS[coords.first];  // Буква строки
            result += COORD_LABELS[coords.second]; // Буква столбца
        }
    }
    return result;
}

// ДЕФРАКЦИОНИРОВАНИЕ: Сборка текста обратно из пар букв ADFGX через Hex-слой
string defractionate(const string& fracText) {
    if (fracText.length() % 4 != 0) {
        throw invalid_argument("Длина должна быть кратна 4 (сейчас: " + 
                               to_string(fracText.length()) + ")");
    }
    
    string hexStr;
    hexStr.reserve(fracText.length() / 2);
    
    for (size_t i = 0; i < fracText.length(); i += 2) {
        char rChar = fracText[i];
        char cChar = fracText[i + 1];
        
        int r = -1, c = -1;
        // Поиск индексов букв в массиве COORD_LABELS
        for (int k = 0; k < 5; ++k) {
            if (COORD_LABELS[k] == rChar) r = k;
            if (COORD_LABELS[k] == cChar) c = k;
        }
        if (r == -1 || c == -1) {
            throw invalid_argument(string("Недопустимые координаты в позиции ") + 
                                  to_string(i) + ": '" + rChar + cChar + "'");
        }
        
        if (r > 3 || c > 3) {
            throw invalid_argument(string("Координата X в позиции ") + 
                                  to_string(i) + ": '" + rChar + cChar + "'");
        }
        
        hexStr += coordsToHexDigit(r, c);
    }
    
    if (hexStr.length() % 2 != 0) {
        throw invalid_argument("Нечётное количество hex-цифр");
    }
    
    string result;
    result.reserve(hexStr.length() / 2);
    for (size_t i = 0; i < hexStr.length(); i += 2) {
        result += static_cast<char>(hexToByte(hexStr.substr(i, 2)));
    }
    
    return result;
}

// СТОЛБЦОВАЯ ПЕРЕСТАНОВКА: Шифрование и расшифрование таблицы по алфавитному порядку ключа
string columnarTransposition(const string& text, const string& keyword, bool encrypt) {
    int kLen = static_cast<int>(keyword.length());
    if (kLen == 0) throw invalid_argument("Ключевое слово не может быть пустым");
    
    int L = static_cast<int>(text.length());
    if (L == 0) return "";
    
    // Формируем алфавитный порядок сортировки для колонок таблицы
    vector<pair<char, int> > keyOrder;
    for (int i = 0; i < kLen; ++i) {
        keyOrder.push_back(make_pair(toupper(static_cast<unsigned char>(keyword[i])), i));
    }
    stable_sort(keyOrder.begin(), keyOrder.end());
    
    if (encrypt) {
        // Запись текста в таблицу построчно
        int numRows = (L + kLen - 1) / kLen;
        vector<string> table(numRows, string(kLen, ' '));
        
        int pos = 0;
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < kLen; ++c) {
                if (pos < L) table[r][c] = text[pos++];
            }
        }
        
        // Считывание текста из таблицы вертикально (по отсортированным колонкам)
        string result;
        for (size_t idx = 0; idx < keyOrder.size(); ++idx) {
            int col = keyOrder[idx].second;
            for (int r = 0; r < numRows; ++r) {
                if (table[r][col] != ' ') {
                    result += table[r][col];
                }
            }
        }
        return result;
        
    } else {
        // Расчет точной длины каждого столбца (с учетом неполной последней строки)
        int numRows = (L + kLen - 1) / kLen;
        int fullCols = L % kLen;  
        if (fullCols == 0) fullCols = kLen; 
        
        vector<int> colLengths(kLen);
        for (int sortedIdx = 0; sortedIdx < kLen; ++sortedIdx) {
            int origCol = keyOrder[sortedIdx].second;
            if (origCol < fullCols) {
                colLengths[origCol] = numRows;
            } else {
                colLengths[origCol] = numRows - 1;
            }
        }
        
        // Восстановление содержимого колонок из шифртекста
        vector<string> cols(kLen);
        int pos = 0;
        for (int sortedIdx = 0; sortedIdx < kLen; ++sortedIdx) {
            int origCol = keyOrder[sortedIdx].second;
            int len = colLengths[origCol];
            
            if (pos + len > L) {
                throw length_error("Ошибка структуры шифртекста");
            }
            
            cols[origCol] = text.substr(pos, len);
            pos += len;
        }
        
        // Сборка исходного текста построчно из восстановленных колонок
        string result;
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < kLen; ++c) {
                if (r < (int)cols[c].length()) {
                    result += cols[c][r];
                }
            }
        }
        return result;
    }
}

// Полный цикл шифрования ADFGX: Сначала разбиение, затем перестановка колонок
string encryptADFGX(const string& plaintext, const string& keyword) {
    string frac = fractionate(plaintext);
    return columnarTransposition(frac, keyword, true);
}

// Полный цикл расшифрования ADFGX: Сначала обратная перестановка, затем сборка байт
string decryptADFGX(const string& ciphertext, const string& keyword) {
    string clean;
    for (size_t i = 0; i < ciphertext.length(); ++i) {
        char c = ciphertext[i];
        if (!isspace(static_cast<unsigned char>(c))) {
            clean += toupper(static_cast<unsigned char>(c));
        }
    }
    
    string frac = columnarTransposition(clean, keyword, false);
    return defractionate(frac);
}