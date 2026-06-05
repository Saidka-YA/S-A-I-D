#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <utility>
#include <clocale>
#include <fstream>

using namespace std;

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

// Символы координат для осей матрицы ADFGX
const char COORD_LABELS[] = {'A', 'D', 'F', 'G', 'X'};

// Конвертация сырого байта в Hex-строку (например, 255 -> "FF")
string byteToHex(unsigned char byte) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(2) << static_cast<int>(byte);
    return ss.str();
}

// Конвертация Hex-пары обратно в байт (например, "FF" -> 255)
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

// Определение координат (строка, столбец) для Hex-символа в сетке 4x4
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

// Получение Hex-символа по его координатам в сетке 4x4
char coordsToHexDigit(int row, int col) {
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        throw invalid_argument("координаты вне диапазона 0-3");
    }
    int val = row * 4 + col;
    return (val < 10) ? ('0' + val) : ('A' + val - 10);
}

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

int main() {
    
}
